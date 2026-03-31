#pragma once
#include <QObject>
#include <QUuid>
#include "../../contracts/UiAction.h"
#include "../../contracts/LogicNotification.h"

class ModuleLogicHandler : public QObject {
    Q_OBJECT
public:
    explicit ModuleLogicHandler(QObject* parent = nullptr);
    ~ModuleLogicHandler() override = default;

    virtual QString moduleName() const = 0;
    virtual void onAction(const UiAction& action);
    virtual void onStateSample(const QString& channel, const QJsonObject& data);

signals:
    void logicNotification(const LogicNotification& notification);

protected:
    void emitNotification(EventType evType, NotificationLevel lvl,
                          const QJsonObject& payload,
                          const QUuid& sourceActionId = {});
};
