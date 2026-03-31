// 文件说明：实现模块逻辑处理器基类，承接动作处理与通知派发的公共流程。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

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
