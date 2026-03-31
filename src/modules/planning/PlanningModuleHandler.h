// 文件说明：规划模块逻辑处理器声明，负责模型加载与规划线维护。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include "../../logic/registry/ModuleLogicHandler.h"
#include "../../logic/scene/SceneGraph.h"
#include "../../logic/scene/nodes/LineNode.h"
#include "../../logic/scene/nodes/ModelNode.h"
#include <QHash>

/// 规划模块逻辑处理器。
///
/// 接收规划界面的用户动作，维护模型节点与规划线节点，并向界面回推场景更新通知。
class PlanningModuleHandler : public ModuleLogicHandler {
    Q_OBJECT
public:
    // --- 模块标识与输入处理 ---
    explicit PlanningModuleHandler(SceneGraph* sceneGraph, QObject* parent = nullptr);

    QString moduleName() const override;
    void onAction(const UiAction& action) override;
    void onStateSample(const QString& channel, const QJsonObject& data) override;

private:
    SceneGraph* sceneGraph_;                                   // 场景图访问入口。
    QHash<QString, QSharedPointer<LineNode>> managedLines_;    // 当前模块管理的规划线集合。
    QHash<QString, QSharedPointer<ModelNode>> managedModels_;  // 当前模块管理的模型集合。

    // --- 内部业务动作 ---
    void loadModel(const QJsonObject& params, const QUuid& sourceActionId);
    void addLine(const QJsonObject& params, const QUuid& sourceActionId);
    void removeLine(const QString& lineId, const QUuid& sourceActionId);
};
