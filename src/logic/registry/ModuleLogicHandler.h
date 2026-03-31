// 文件说明：模块逻辑处理器基类声明，负责接收动作并发出逻辑通知。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QObject>
#include <QUuid>
#include "../../contracts/UiAction.h"
#include "../../contracts/LogicNotification.h"

/// 模块逻辑处理器基类。
///
/// 负责承接来自界面的动作与外部状态采样，并向上层发出标准化通知。
class ModuleLogicHandler : public QObject {
    Q_OBJECT
public:
    // --- 模块标识与输入处理 ---
    explicit ModuleLogicHandler(QObject* parent = nullptr);
    ~ModuleLogicHandler() override = default;

    virtual QString moduleName() const = 0;
    virtual void onAction(const UiAction& action);
    virtual void onStateSample(const QString& channel, const QJsonObject& data);

signals:
    void logicNotification(const LogicNotification& notification);

protected:
    // 统一封装逻辑通知，减少各模块重复拼装通知对象。
    void emitNotification(EventType evType, NotificationLevel lvl,
                          const QJsonObject& payload,
                          TargetScope scope,
                          const QUuid& sourceActionId = {});
};
