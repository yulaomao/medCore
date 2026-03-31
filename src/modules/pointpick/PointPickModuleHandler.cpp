#include "PointPickModuleHandler.h"
#include <QJsonArray>

PointPickModuleHandler::PointPickModuleHandler(SceneGraph* sceneGraph, QObject* parent)
    : ModuleLogicHandler(parent)
    , sceneGraph_(sceneGraph)
{}

QString PointPickModuleHandler::moduleName() const { return "pointpick"; }

void PointPickModuleHandler::onAction(const UiAction& action) {
    if (action.module != moduleName()) return;
    if (action.actionType != ActionType::UserInput) return;

    const QString cmd = action.payload["command"].toString();
    if (cmd == "addPoint") {
        addPoint(action.payload, action.actionId);
    } else if (cmd == "removePoint") {
        removePoint(action.payload["pointId"].toString(), action.actionId);
    } else if (cmd == "clearPoints") {
        clearPoints(action.actionId);
    }
}

void PointPickModuleHandler::onStateSample(const QString& channel, const QJsonObject& data) {
    if (channel != "pointpick.positions") return;
    QJsonArray pts = data["points"].toArray();
    for (const auto& ptVal : pts) {
        auto ptObj = ptVal.toObject();
        auto node = QSharedPointer<PointNode>::create();
        node->setPosition(QVector3D(ptObj["x"].toDouble(),
                                    ptObj["y"].toDouble(),
                                    ptObj["z"].toDouble()));
        node->setLabel(ptObj["label"].toString());
        const QString id = node->nodeId().toString();
        managedPoints_[id] = node;
        sceneGraph_->addNode(node);
    }
    QJsonObject payload;
    payload["count"] = managedPoints_.size();
    emitNotification(EventType::SceneUpdated, NotificationLevel::Info,
                     payload, TargetScope::CurrentModule);
}

void PointPickModuleHandler::addPoint(const QJsonObject& params, const QUuid& sourceActionId) {
    auto node = QSharedPointer<PointNode>::create();
    node->setPosition(QVector3D(params["x"].toDouble(),
                                 params["y"].toDouble(),
                                 params["z"].toDouble()));
    node->setLabel(params["label"].toString());
    if (params.contains("color")) node->setColor(QColor(params["color"].toString()));
    if (params.contains("radius")) node->setRadius(params["radius"].toDouble());

    const QString id = node->nodeId().toString();
    managedPoints_[id] = node;
    sceneGraph_->addNode(node);

    QJsonObject payload;
    payload["pointId"] = id;
    payload["count"]   = managedPoints_.size();
    emitNotification(EventType::SceneUpdated, NotificationLevel::Info,
                     payload, TargetScope::CurrentModule, sourceActionId);
}

void PointPickModuleHandler::removePoint(const QString& pointId, const QUuid& sourceActionId) {
    managedPoints_.remove(pointId);
    sceneGraph_->removeNode(pointId);

    QJsonObject payload;
    payload["pointId"] = pointId;
    payload["count"]   = managedPoints_.size();
    emitNotification(EventType::SceneUpdated, NotificationLevel::Info,
                     payload, TargetScope::CurrentModule, sourceActionId);
}

void PointPickModuleHandler::clearPoints(const QUuid& sourceActionId) {
    for (const auto& id : managedPoints_.keys())
        sceneGraph_->removeNode(id);
    managedPoints_.clear();

    QJsonObject payload;
    payload["count"] = 0;
    emitNotification(EventType::SceneUpdated, NotificationLevel::Info,
                     payload, TargetScope::CurrentModule, sourceActionId);
}
