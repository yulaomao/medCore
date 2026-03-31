// 文件说明：逻辑运行时声明，负责衔接界面网关、工作流与模块逻辑。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QObject>
#include <QSharedPointer>
#include "../../logic/gateway/ILogicGateway.h"
#include "../../logic/registry/ModuleLogicRegistry.h"
#include "../../logic/workflow/WorkflowStateMachine.h"
#include "../../logic/scene/SceneGraph.h"

/// 逻辑运行时。
///
/// 作为 ILogicGateway 的具体实现，负责连接模块逻辑、工作流状态机和场景图。
class LogicRuntime : public ILogicGateway {
    Q_OBJECT
public:
    // --- 构造与网关接口实现 ---
    explicit LogicRuntime(ModuleLogicRegistry* registry,
                          WorkflowStateMachine* workflow,
                          SceneGraph* sceneGraph,
                          QObject* parent = nullptr);
    ~LogicRuntime() override;

    void sendAction(const UiAction& action) override;
    void subscribeNotification(QObject* receiver, const char* slot) override;
    void unsubscribeNotification(QObject* receiver) override;
    QString getConnectionState() const override;
    void requestResync() override;

    // --- 运行时生命周期 ---
    void start();
    void stop();

signals:
    // notificationReceived 信号由 ILogicGateway 基类继承提供

public slots:
    // --- 运行时调度入口 ---
    void processAction(const UiAction& action);
    void onStateSample(const QString& channel, const QJsonObject& data);

private slots:
    void onHandlerNotification(const LogicNotification& notification);

private:
    void emitWorkflowBlocked(const UiAction& action, const QString& targetModule, const QString& reason);
    QString targetModuleForAction(const UiAction& action) const;

    ModuleLogicRegistry* registry_;         // 模块逻辑注册表。
    WorkflowStateMachine* workflow_;        // 工作流状态机。
    SceneGraph* sceneGraph_;                // 场景图。
    QString connectionState_{"Disconnected"}; // 当前逻辑连接状态文本。
};
