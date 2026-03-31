// 文件说明：点采集模块逻辑处理器声明，负责点节点的创建、删除与清空。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include "../../logic/registry/ModuleLogicHandler.h"
#include "../../logic/scene/SceneGraph.h"
#include "../../logic/scene/nodes/PointNode.h"
#include <QHash>

/// 点采集模块逻辑处理器。
///
/// 负责创建、删除和清空 PointNode，并同步点列表状态。
class PointPickModuleHandler : public ModuleLogicHandler {
    Q_OBJECT
public:
    explicit PointPickModuleHandler(SceneGraph* sceneGraph, QObject* parent = nullptr);

    QString moduleName() const override;
    void onAction(const UiAction& action) override;
    void onStateSample(const QString& channel, const QJsonObject& data) override;

private:
    SceneGraph* sceneGraph_;                                  // 场景图访问入口。
    QHash<QString, QSharedPointer<PointNode>> managedPoints_; // 当前模块管理的点节点集合。

    // --- 内部业务动作 ---
    void addPoint(const QJsonObject& params, const QUuid& sourceActionId);
    void removePoint(const QString& pointId, const QUuid& sourceActionId);
    void clearPoints(const QUuid& sourceActionId);
};
