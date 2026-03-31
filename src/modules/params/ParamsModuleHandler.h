// 文件说明：参数模块逻辑处理器声明，负责维护当前参数快照。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include "../../logic/registry/ModuleLogicHandler.h"
#include "../../logic/scene/SceneGraph.h"

/// 参数模块逻辑处理器。
///
/// 维护当前参数快照，并负责参数同步与参数变更通知。
class ParamsModuleHandler : public ModuleLogicHandler {
    Q_OBJECT
public:
    explicit ParamsModuleHandler(SceneGraph* sceneGraph, QObject* parent = nullptr);

    QString moduleName() const override;
    void onAction(const UiAction& action) override;
    void onStateSample(const QString& channel, const QJsonObject& data) override;

private:
    SceneGraph* sceneGraph_;      // 场景图访问入口，便于后续扩展。
    QJsonObject currentParams_;   // 当前参数快照。
};
