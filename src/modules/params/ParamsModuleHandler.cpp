// 文件说明：实现参数模块的逻辑处理，负责参数同步与更新。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "ParamsModuleHandler.h"
#include <QDebug>

ParamsModuleHandler::ParamsModuleHandler(SceneGraph* sceneGraph, QObject* parent)
    : ModuleLogicHandler(parent)
    , sceneGraph_(sceneGraph)
{}

QString ParamsModuleHandler::moduleName() const { return "params"; }

void ParamsModuleHandler::onAction(const UiAction& action) {
    if (action.actionType != ActionType::UserInput) return;
    if (action.module != moduleName()) return;

    // 将收到的参数负载合并到当前参数对象中
    for (auto it = action.payload.begin(); it != action.payload.end(); ++it)
        currentParams_[it.key()] = it.value();

    QJsonObject notifPayload;
    notifPayload["params"] = currentParams_;
    emitNotification(EventType::StateChanged, NotificationLevel::Info,
                     notifPayload, TargetScope::CurrentModule,
                     action.actionId);
}

void ParamsModuleHandler::onStateSample(const QString& channel, const QJsonObject& data) {
    if (!channel.startsWith("params.")) return;
    for (auto it = data.begin(); it != data.end(); ++it)
        currentParams_[it.key()] = it.value();

    QJsonObject notifPayload;
    notifPayload["params"] = currentParams_;
    notifPayload["channel"] = channel;
    emitNotification(EventType::DataArrived, NotificationLevel::Info,
                     notifPayload, TargetScope::CurrentModule);
}
