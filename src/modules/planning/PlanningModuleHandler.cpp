#include "PlanningModuleHandler.h"

PlanningModuleHandler::PlanningModuleHandler(SceneGraph* sceneGraph, QObject* parent)
    : ModuleLogicHandler(parent)
    , sceneGraph_(sceneGraph)
{}

QString PlanningModuleHandler::moduleName() const { return "planning"; }

void PlanningModuleHandler::onAction(const UiAction& action) {
    if (action.module != moduleName()) return;
    if (action.actionType != ActionType::UserInput) return;

    const QString cmd = action.payload["command"].toString();
    if (cmd == "loadModel") {
        loadModel(action.payload, action.actionId);
    } else if (cmd == "addLine") {
        addLine(action.payload, action.actionId);
    } else if (cmd == "removeLine") {
        removeLine(action.payload["lineId"].toString(), action.actionId);
    }
}

void PlanningModuleHandler::onStateSample(const QString& channel, const QJsonObject& data) {
    if (channel != "planning.state") return;
    QJsonObject notifPayload = data;
    emitNotification(EventType::DataArrived, NotificationLevel::Info,
                     notifPayload, TargetScope::CurrentModule);
}

void PlanningModuleHandler::loadModel(const QJsonObject& params, const QUuid& sourceActionId) {
    auto node = QSharedPointer<ModelNode>::create();
    node->setFilePath(params["filePath"].toString());
    if (params.contains("color")) node->setColor(QColor(params["color"].toString()));
    node->setOpacity(params["opacity"].toDouble(1.0));

    const QString id = node->nodeId().toString();
    managedModels_[id] = node;
    sceneGraph_->addNode(node);

    QJsonObject payload;
    payload["modelId"] = id;
    payload["filePath"] = node->filePath();
    emitNotification(EventType::SceneUpdated, NotificationLevel::Info,
                     payload, TargetScope::CurrentModule, sourceActionId);
}

void PlanningModuleHandler::addLine(const QJsonObject& params, const QUuid& sourceActionId) {
    auto node = QSharedPointer<LineNode>::create();
    auto decodeVec = [](const QJsonObject& o) {
        return QVector3D(o["x"].toDouble(), o["y"].toDouble(), o["z"].toDouble());
    };
    if (params.contains("start")) node->setStartPoint(decodeVec(params["start"].toObject()));
    if (params.contains("end"))   node->setEndPoint(decodeVec(params["end"].toObject()));
    if (params.contains("color")) node->setColor(QColor(params["color"].toString()));
    if (params.contains("lineWidth")) node->setLineWidth(params["lineWidth"].toDouble());

    const QString id = node->nodeId().toString();
    managedLines_[id] = node;
    sceneGraph_->addNode(node);

    QJsonObject payload;
    payload["lineId"] = id;
    payload["count"]  = managedLines_.size();
    emitNotification(EventType::SceneUpdated, NotificationLevel::Info,
                     payload, TargetScope::CurrentModule, sourceActionId);
}

void PlanningModuleHandler::removeLine(const QString& lineId, const QUuid& sourceActionId) {
    managedLines_.remove(lineId);
    sceneGraph_->removeNode(lineId);

    QJsonObject payload;
    payload["lineId"] = lineId;
    payload["count"]  = managedLines_.size();
    emitNotification(EventType::SceneUpdated, NotificationLevel::Info,
                     payload, TargetScope::CurrentModule, sourceActionId);
}
