#include "ModuleLogicHandler.h"

ModuleLogicHandler::ModuleLogicHandler(QObject* parent) : QObject(parent) {}

void ModuleLogicHandler::onAction(const UiAction& /*action*/) {}

void ModuleLogicHandler::onStateSample(const QString& /*channel*/, const QJsonObject& /*data*/) {}

void ModuleLogicHandler::emitNotification(EventType evType, NotificationLevel lvl,
                                           const QJsonObject& payload,
                                           const QUuid& sourceActionId)
{
    auto n = LogicNotification::create(evType, lvl, payload,
                                       TargetScope::Module,
                                       {moduleName()},
                                       sourceActionId);
    emit logicNotification(n);
}
