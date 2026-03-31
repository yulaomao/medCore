#include "LogicRuntime.h"
#include <QMetaObject>

LogicRuntime::LogicRuntime(ModuleLogicRegistry* registry,
                            WorkflowStateMachine* workflow,
                            SceneGraph* sceneGraph,
                            QObject* parent)
    : ILogicGateway(parent)
    , registry_(registry)
    , workflow_(workflow)
    , sceneGraph_(sceneGraph)
{
    // Connect all handler notifications
    for (const auto& handler : registry_->allHandlers()) {
        connect(handler.data(), &ModuleLogicHandler::logicNotification,
                this, &LogicRuntime::onHandlerNotification,
                Qt::QueuedConnection);
    }

    connect(workflow_, &WorkflowStateMachine::moduleChanged, this,
            [this](const QString& oldModule, const QString& newModule) {
                QJsonObject payload;
                payload["oldModule"] = oldModule;
                payload["module"] = newModule;
                payload["currentModule"] = newModule;
                emit notificationReceived(
                    LogicNotification::create(EventType::ModuleChanged,
                                              NotificationLevel::Info,
                                              payload,
                                              TargetScope::Shell));
                emit notificationReceived(
                    LogicNotification::create(EventType::WorkflowChanged,
                                              NotificationLevel::Info,
                                              payload,
                                              TargetScope::Shell));
            },
            Qt::QueuedConnection);
}

LogicRuntime::~LogicRuntime() {
    stop();
}

void LogicRuntime::start() {
    if (connectionState_ != "Connected") {
        connectionState_ = "Connected";
        emit connectionStateChanged(connectionState_);
    }
}

void LogicRuntime::stop() {
    if (connectionState_ != "Disconnected") {
        connectionState_ = "Disconnected";
        emit connectionStateChanged(connectionState_);
    }
}

void LogicRuntime::sendAction(const UiAction& action) {
    QMetaObject::invokeMethod(this, "processAction",
                              Qt::QueuedConnection,
                              Q_ARG(UiAction, action));
}

void LogicRuntime::subscribeNotification(QObject* receiver, const char* slot) {
    connect(this, SIGNAL(notificationReceived(LogicNotification)),
            receiver, slot, Qt::QueuedConnection);
}

void LogicRuntime::unsubscribeNotification(QObject* receiver) {
    disconnect(this, SIGNAL(notificationReceived(LogicNotification)), receiver, nullptr);
}

QString LogicRuntime::getConnectionState() const {
    return connectionState_;
}

void LogicRuntime::requestResync() {
    auto action = UiAction::create(ActionType::SyncRequest, QString(), QJsonObject());
    sendAction(action);
}

void LogicRuntime::processAction(const UiAction& action) {
    if (!workflow_) return;

    const QString targetModule = targetModuleForAction(action);

    if (action.actionType == ActionType::NextStep) {
        const QStringList sequence = workflow_->workflowSequence();
        const int currentIndex = sequence.indexOf(workflow_->currentModule());
        if (currentIndex >= 0 && currentIndex + 1 < sequence.size()) {
            const QString nextModule = sequence.at(currentIndex + 1);
            if (!workflow_->advance(nextModule)) {
                emitWorkflowBlocked(action, nextModule, QStringLiteral("next_step_blocked"));
            }
        }
        return;
    }

    if (action.actionType == ActionType::PreviousStep) {
        const QStringList sequence = workflow_->workflowSequence();
        const int currentIndex = sequence.indexOf(workflow_->currentModule());
        if (currentIndex > 0) {
            const QString previousModule = sequence.at(currentIndex - 1);
            if (!workflow_->advance(previousModule)) {
                emitWorkflowBlocked(action, previousModule, QStringLiteral("previous_step_blocked"));
            }
        }
        return;
    }

    if (action.actionType == ActionType::NavigateToModule ||
        action.actionType == ActionType::RequestSwitchModule) {
        if (targetModule.isEmpty()) {
            emitWorkflowBlocked(action, targetModule, QStringLiteral("target_module_missing"));
            return;
        }
        if (!workflow_->advance(targetModule)) {
            emitWorkflowBlocked(action, targetModule, QStringLiteral("module_not_enterable"));
        }
        return;
    }

    auto handler = registry_->getHandler(action.module);
    if (handler) {
        handler->onAction(action);
    }
}

void LogicRuntime::onStateSample(const QString& channel, const QJsonObject& data) {
    const QString moduleName = channel.section('.', 0, 0);
    if (!moduleName.isEmpty()) {
        auto targetedHandler = registry_->getHandler(moduleName);
        if (targetedHandler) {
            targetedHandler->onStateSample(channel, data);
            return;
        }
    }

    for (const auto& handler : registry_->allHandlers()) {
        handler->onStateSample(channel, data);
    }
}

void LogicRuntime::onHandlerNotification(const LogicNotification& notification) {
    emit notificationReceived(notification);
}

void LogicRuntime::emitWorkflowBlocked(const UiAction& action,
                                       const QString& targetModule,
                                       const QString& reason)
{
    QJsonObject payload;
    payload["requestedModule"] = targetModule;
    payload["reason"] = reason;
    payload["currentModule"] = workflow_ ? workflow_->currentModule() : QString();
    emit notificationReceived(
        LogicNotification::create(EventType::ModuleError,
                                  NotificationLevel::Error,
                                  payload,
                                  TargetScope::Shell,
                                  {},
                                  action.actionId));
}

QString LogicRuntime::targetModuleForAction(const UiAction& action) const {
    const QString payloadTarget = action.payload["targetModule"].toString();
    if (!payloadTarget.isEmpty()) return payloadTarget;
    return action.module;
}
