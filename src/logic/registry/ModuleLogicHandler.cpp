#include "ModuleLogicHandler.h"

ModuleLogicHandler::ModuleLogicHandler(QObject* parent) : QObject(parent) {}

void ModuleLogicHandler::onAction(const UiAction& /*action*/) {}

void ModuleLogicHandler::onStateSample(const QString& /*channel*/, const QJsonObject& /*data*/) {}

void ModuleLogicHandler::emitNotification(EventType evType, NotificationLevel lvl,
                                          const QJsonObject& payload,
                                          TargetScope scope,
                                          const QUuid& sourceActionId)
{
    QStringList targetModules;
    if (scope == TargetScope::ModuleList) {
        targetModules = {moduleName()};
    }

    auto n = LogicNotification::create(evType, lvl, payload,
                                       scope,
                                       targetModules,
                                       sourceActionId);
    emit logicNotification(n);
}
