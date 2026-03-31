// 文件说明：实现模块协调器基类，统一模块界面、动作发送和通知处理入口。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "ModuleCoordinator.h"

ModuleCoordinator::ModuleCoordinator(QObject* parent) : QObject(parent) {}

void ModuleCoordinator::onModuleNotification(const LogicNotification& /*notification*/) {}

void ModuleCoordinator::activate() {}

void ModuleCoordinator::deactivate() {}

void ModuleCoordinator::setGateway(ILogicGateway* gateway) {
    gateway_ = gateway;
}

QWidget* ModuleCoordinator::attachmentWidget(AttachmentSlot slot) const {
    return slot == AttachmentSlot::Right ? rightAttachment_.data()
                                         : bottomAttachment_.data();
}

void ModuleCoordinator::sendAction(ActionType type, const QJsonObject& payload) {
    if (!gateway_) return;
    gateway_->sendAction(UiAction::create(type, moduleName(), payload));
}

void ModuleCoordinator::setAttachmentWidget(AttachmentSlot slot, QWidget* widget) {
    if (slot == AttachmentSlot::Right) {
        rightAttachment_ = widget;
    } else {
        bottomAttachment_ = widget;
    }
}
