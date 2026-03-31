#include "ModuleCoordinator.h"

ModuleCoordinator::ModuleCoordinator(QObject* parent) : QObject(parent) {}

void ModuleCoordinator::onModuleNotification(const LogicNotification& /*notification*/) {}

void ModuleCoordinator::activate() {}

void ModuleCoordinator::deactivate() {}

void ModuleCoordinator::setGateway(ILogicGateway* gateway) {
    gateway_ = gateway;
}

void ModuleCoordinator::sendAction(ActionType type, const QJsonObject& payload) {
    if (!gateway_) return;
    gateway_->sendAction(UiAction::create(type, moduleName(), payload));
}
