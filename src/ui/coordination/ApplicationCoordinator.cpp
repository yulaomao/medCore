#include "ApplicationCoordinator.h"
#include "../../shell/MainWindow.h"
#include <QDebug>

ApplicationCoordinator::ApplicationCoordinator(ILogicGateway* gateway,
                                                 PageManager* pageManager,
                                                 GlobalUiManager* uiManager,
                                                 QObject* parent)
    : QObject(parent)
    , gateway_(gateway)
    , pageManager_(pageManager)
    , uiManager_(uiManager)
{}

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

    if (coordinators_.contains(moduleName))
        coordinators_[moduleName]->activate();

    emit moduleActivated(moduleName);
}

void ApplicationCoordinator::connectShellSignals(MainWindow* mainWindow) {
    if (!mainWindow) return;
    connect(mainWindow, &MainWindow::shutdownRequested, [this]() {
        if (gateway_)
            gateway_->sendAction(UiAction::create(ActionType::Shutdown, QString(), QJsonObject()));
    });
}

void ApplicationCoordinator::onNotification(const LogicNotification& notification) {
    if (notification.targetScope == TargetScope::All) {
        for (auto& coord : coordinators_)
            coord->onModuleNotification(notification);
    } else if (notification.targetScope == TargetScope::Module) {
        for (const auto& mod : notification.targetModules) {
            if (coordinators_.contains(mod))
                coordinators_[mod]->onModuleNotification(notification);
        }
    } else if (notification.targetScope == TargetScope::Shell) {
        // Shell-level notifications (e.g. workflow changes)
        if (notification.eventType == EventType::WorkflowAdvanced) {
            const QString nextModule = notification.payload["module"].toString();
            if (!nextModule.isEmpty()) activateModule(nextModule);
        }
    }
}
