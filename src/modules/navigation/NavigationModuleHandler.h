// 文件说明：导航模块逻辑处理器声明，负责跟踪位姿节点的创建与更新。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include "../../logic/registry/ModuleLogicHandler.h"
#include "../../logic/scene/SceneGraph.h"
#include "../../logic/scene/nodes/TransformNode.h"
#include <QSharedPointer>

/// 导航模块逻辑处理器。
///
/// 负责维护导航位姿对应的 TransformNode，并同步实时导航状态。
class NavigationModuleHandler : public ModuleLogicHandler {
    Q_OBJECT
public:
    // --- 模块标识与输入处理 ---
    explicit NavigationModuleHandler(SceneGraph* sceneGraph, QObject* parent = nullptr);

    QString moduleName() const override;
    void onAction(const UiAction& action) override;
    void onStateSample(const QString& channel, const QJsonObject& data) override;

private:
    SceneGraph* sceneGraph_;                  // 场景图访问入口。
    QSharedPointer<TransformNode> poseNode_;  // 当前导航位姿对应的节点对象。
    QString poseNodeId_;                      // 位姿节点的稳定标识。
};
