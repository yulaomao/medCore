#include "ApplicationCoordinator.h"
#include "../../shell/MainWindow.h"
#include "../../shell/WorkspaceShell.h"
#include <QDebug>

ApplicationCoordinator::ApplicationCoordinator(ILogicGateway* gateway,
                                                 PageManager* pageManager,
                                                 GlobalUiManager* uiManager,
                                                 QObject* parent)
    : QObject(parent)
    , gateway_(gateway)
    , pageManager_(pageManager)
    , uiManager_(uiManager)
{
    if (gateway_) {
        connect(gateway_, &ILogicGateway::connectionStateChanged,
                this, &ApplicationCoordinator::onConnectionStateChanged);
    }
}

void ApplicationCoordinator::registerModuleCoordinator(QSharedPointer<ModuleCoordinator> coordinator) {
    if (!coordinator) return;
    const QString name = coordinator->moduleName();
    coordinator->setGateway(gateway_);
    coordinators_[name] = coordinator;

    // Register main page with page manager
    QWidget* page = coordinator->mainPage();
    if (page) pageManager_->registerPage(name, page);
}

void ApplicationCoordinator::activateModule(const QString& moduleName) {
    if (activeModule_ == moduleName) return;

    if (coordinators_.contains(activeModule_))
        coordinators_[activeModule_]->deactivate();

    activeModule_ = moduleName;
    pageManager_->showPage(moduleName);
    mountModuleAttachments(moduleName);

    if (coordinators_.contains(moduleName))
        coordinators_[moduleName]->activate();

    emit moduleActivated(moduleName);
}

void ApplicationCoordinator::connectShellSignals(MainWindow* mainWindow) {
    if (!mainWindow) return;
    workspaceShell_ = mainWindow->workspaceShell();
    connect(mainWindow, &MainWindow::shutdownRequested, [this]() {
        if (gateway_)
            gateway_->sendAction(UiAction::create(ActionType::Shutdown, QString(), QJsonObject()));
    });
}

void ApplicationCoordinator::onNotification(const LogicNotification& notification) {
    if (notification.targetScope == TargetScope::AllModules) {
        for (auto& coord : coordinators_)
            coord->onModuleNotification(notification);
    } else if (notification.targetScope == TargetScope::CurrentModule) {
        if (coordinators_.contains(activeModule_))
            coordinators_[activeModule_]->onModuleNotification(notification);
    } else if (notification.targetScope == TargetScope::ModuleList) {
        for (const auto& mod : notification.targetModules) {
            if (coordinators_.contains(mod))
                coordinators_[mod]->onModuleNotification(notification);
        }
    } else if (notification.targetScope == TargetScope::Shell) {
        const QString nextModule = notification.payload["currentModule"].toString(
            notification.payload["module"].toString());
        if ((notification.eventType == EventType::ModuleChanged ||
             notification.eventType == EventType::WorkflowChanged ||
             notification.eventType == EventType::PageChanged) &&
            !nextModule.isEmpty())
        {
            activateModule(nextModule);
        }
        if (notification.eventType == EventType::ConnectionStateChanged)
            onConnectionStateChanged(notification.payload["state"].toString());
    }
}

void ApplicationCoordinator::onConnectionStateChanged(const QString& state) {
    if (!uiManager_) return;
    uiManager_->showToast(tr("Connection state: %1").arg(state), 2000);
}

void ApplicationCoordinator::mountModuleAttachments(const QString& moduleName) {
    if (!workspaceShell_) return;

    QWidget* rightWidget = nullptr;
    QWidget* bottomWidget = nullptr;
    if (coordinators_.contains(moduleName)) {
        auto coordinator = coordinators_[moduleName];
        rightWidget = coordinator->attachmentWidget(ModuleCoordinator::AttachmentSlot::Right);
        bottomWidget = coordinator->attachmentWidget(ModuleCoordinator::AttachmentSlot::Bottom);
    }

    workspaceShell_->setRightArea(rightWidget);
    workspaceShell_->setBottomArea(bottomWidget);
}
