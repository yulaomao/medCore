#pragma once
#include <QObject>
#include <QWidget>
#include "../../logic/gateway/ILogicGateway.h"

class ModuleCoordinator : public QObject {
    Q_OBJECT
public:
    explicit ModuleCoordinator(QObject* parent = nullptr);
    ~ModuleCoordinator() override = default;

    virtual QString moduleName() const = 0;
    virtual QWidget* mainPage() = 0;

    virtual void onModuleNotification(const LogicNotification& notification);
    virtual void activate();
    virtual void deactivate();

    void setGateway(ILogicGateway* gateway);

protected:
    void sendAction(ActionType type, const QJsonObject& payload);

    ILogicGateway* gateway_{nullptr};
};
