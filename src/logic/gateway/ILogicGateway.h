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
