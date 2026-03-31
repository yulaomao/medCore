// 文件说明：定义界面层与逻辑层之间的网关接口，统一动作发送与通知订阅能力。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#pragma once
#include <QObject>
#include "../../contracts/UiAction.h"
#include "../../contracts/LogicNotification.h"

class ILogicGateway : public QObject {
    Q_OBJECT
public:
    explicit ILogicGateway(QObject* parent = nullptr) : QObject(parent) {}
    ~ILogicGateway() override = default;

    virtual void sendAction(const UiAction& action) = 0;
    virtual void subscribeNotification(QObject* receiver, const char* slot) = 0;
    virtual void unsubscribeNotification(QObject* receiver) = 0;
    virtual QString getConnectionState() const = 0;
    virtual void requestResync() = 0;

signals:
    void notificationReceived(const LogicNotification& notification);
    void connectionStateChanged(const QString& state);
};
