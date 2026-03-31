// 文件说明：实现导航模块逻辑，负责跟踪位姿节点更新与同步。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "NavigationModuleHandler.h"

NavigationModuleHandler::NavigationModuleHandler(SceneGraph* sceneGraph, QObject* parent)
    : ModuleLogicHandler(parent)
    , sceneGraph_(sceneGraph)
{
    // 为被跟踪位姿创建一个持久化的变换节点
    poseNode_ = QSharedPointer<TransformNode>::create();
    poseNodeId_ = poseNode_->nodeId().toString();
    sceneGraph_->addNode(poseNode_);
}

QString NavigationModuleHandler::moduleName() const { return "navigation"; }

void NavigationModuleHandler::onAction(const UiAction& action) {
    if (action.module != moduleName()) return;
    if (action.actionType == ActionType::SyncRequest) {
        QJsonObject payload;
        payload["poseNodeId"] = poseNodeId_;
        emitNotification(EventType::ModuleReady, NotificationLevel::Info,
                         payload, TargetScope::CurrentModule,
                         action.actionId);
    }
}

void NavigationModuleHandler::onStateSample(const QString& channel, const QJsonObject& data) {
    if (channel != "navigation.pose") return;

    poseNode_->beginModify();
    if (data.contains("translation")) {
        auto t = data["translation"].toObject();
        poseNode_->setTranslation(QVector3D(t["x"].toDouble(), t["y"].toDouble(), t["z"].toDouble()));
    }
    if (data.contains("rotation")) {
        auto r = data["rotation"].toObject();
        poseNode_->setRotation(QVector3D(r["x"].toDouble(), r["y"].toDouble(), r["z"].toDouble()));
    }
    poseNode_->endModify();

    QJsonObject payload;
    payload["channel"]    = channel;
    payload["poseNodeId"] = poseNodeId_;
    payload["translation"] = data.value("translation");
    payload["rotation"]    = data.value("rotation");
    emitNotification(EventType::SceneUpdated, NotificationLevel::Info,
                     payload, TargetScope::CurrentModule);
}
