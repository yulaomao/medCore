// 文件说明：实现软件初始化器基类的默认配置与公共访问接口。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "BaseSoftwareInitializer.h"
#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QStackedWidget>
#include "../../communication/routing/MessageRouter.h"
#include "../../logic/runtime/LogicRuntime.h"
#include "../../logic/registry/ModuleLogicRegistry.h"
#include "../../logic/scene/SceneGraph.h"
#include "../../logic/workflow/WorkflowStateMachine.h"
#include "../../communication/hub/CommunicationHub.h"
#include "../../shell/MainWindow.h"
#include "../../shell/WorkspaceShell.h"
#include "../../ui/coordination/ApplicationCoordinator.h"
#include "../../ui/pages/PageManager.h"
#include "../../ui/globalui/GlobalUiManager.h"

bool SoftwareBootstrapResult::isValid() const {
    return mainWindow != nullptr &&
           appCoordinator != nullptr &&
           !logicRuntime.isNull() &&
           !communicationHub.isNull();
}

BaseSoftwareInitializer::BaseSoftwareInitializer(QObject* parent) : QObject(parent) {}

SoftwareBootstrapResult BaseSoftwareInitializer::build(QApplication& application) {
    SoftwareBootstrapResult result;
    const QStringList modules = enabledModules();

    auto* sceneGraph = new SceneGraph(&application);
    auto* workflow = new WorkflowStateMachine(workflowSequence(), initialModule(), &application);
    workflow->setEnterableModules(modules);

    auto* registry = new ModuleLogicRegistry(&application);
    registerModuleHandlers(modules, registry, sceneGraph);

    logicRuntime_ = QSharedPointer<LogicRuntime>::create(registry, workflow, sceneGraph);
    logicRuntime_->start();

    auto* messageRouter = new MessageRouter(&application);
    communicationHub_ = QSharedPointer<CommunicationHub>::create();
    communicationHub_->setMessageRouter(messageRouter);
    communicationHub_->setLogicRuntime(logicRuntime_.data());
    configureCommunication(messageRouter, communicationHub_.data(), &application);
    communicationHub_->start();

    auto* stackedWidget = new QStackedWidget();
    auto* pageManager = new PageManager(stackedWidget, &application);
    auto* uiManager = new GlobalUiManager(&application);
    auto* appCoordinator = new ApplicationCoordinator(logicRuntime_.data(), pageManager, uiManager, &application);

    const QList<ModuleUiRegistration> registrations = createModuleUiRegistrations(modules, &application);
    for (const ModuleUiRegistration& registration : registrations) {
        for (const SceneWindowRegistration& windowRegistration : registration.sceneWindows) {
            if (!windowRegistration.windowId.isEmpty() && windowRegistration.window)
                uiManager->register3dWindow(windowRegistration.windowId, windowRegistration.window);
        }

        if (registration.coordinator)
            appCoordinator->registerModuleCoordinator(registration.coordinator);
    }

    logicRuntime_->subscribeNotification(appCoordinator, SLOT(onNotification(LogicNotification)));

    auto* workspaceShell = new WorkspaceShell();
    workspaceShell->setPageArea(stackedWidget);
    for (const QString& moduleName : workflowSequence()) {
        auto* action = new QAction(moduleName, workspaceShell);
        QObject::connect(action, &QAction::triggered, [appCoordinator, moduleName]() {
            appCoordinator->requestModuleActivation(moduleName);
        });
        workspaceShell->addToolbarAction(action);
    }

    auto* mainWindow = new MainWindow(workspaceShell);
    appCoordinator->connectShellSignals(mainWindow);

    QObject::connect(&application, &QCoreApplication::aboutToQuit,
                     this, &BaseSoftwareInitializer::shutdown);

    appCoordinator->activateModule(workflow->currentModule());

    result.mainWindow = mainWindow;
    result.appCoordinator = appCoordinator;
    result.logicRuntime = logicRuntime_;
    result.communicationHub = communicationHub_;
    emit initializationComplete();
    return result;
}

void BaseSoftwareInitializer::shutdown() {
    if (communicationHub_)
        communicationHub_->stop();
    if (logicRuntime_)
        logicRuntime_->stop();

    emit shutdownComplete();
}

QStringList BaseSoftwareInitializer::enabledModules() const {
    return workflowSequence();
}

void BaseSoftwareInitializer::configureCommunication(MessageRouter* messageRouter,
                                                    CommunicationHub* communicationHub,
                                                    QObject* parent) {
    Q_UNUSED(messageRouter);
    Q_UNUSED(communicationHub);
    Q_UNUSED(parent);
}

QSharedPointer<LogicRuntime> BaseSoftwareInitializer::logicRuntime() const {
    return logicRuntime_;
}

QSharedPointer<CommunicationHub> BaseSoftwareInitializer::communicationHub() const {
    return communicationHub_;
}
