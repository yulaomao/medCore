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
}

LogicRuntime::~LogicRuntime() {
    stop();
}

void LogicRuntime::start() {
    if (connectionState_ != "Connected") {
        connectionState_ = "Connected";
    }
}

void LogicRuntime::stop() {
    if (connectionState_ != "Disconnected") {
        connectionState_ = "Disconnected";
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
    if (action.actionType == ActionType::NavigateToModule) {
        workflow_->advance(action.module);
    }

    auto handler = registry_->getHandler(action.module);
    if (handler) {
        handler->onAction(action);
    }
}

void LogicRuntime::onStateSample(const QString& channel, const QJsonObject& data) {
    for (const auto& handler : registry_->allHandlers()) {
        handler->onStateSample(channel, data);
    }
}

void LogicRuntime::onHandlerNotification(const LogicNotification& notification) {
    emit notificationReceived(notification);
}
