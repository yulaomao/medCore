// 文件说明：实现软件初始化器基类的默认配置与公共访问接口。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "BaseSoftwareInitializer.h"
#include "../../logic/runtime/LogicRuntime.h"
#include "../../communication/hub/CommunicationHub.h"

BaseSoftwareInitializer::BaseSoftwareInitializer(QObject* parent) : QObject(parent) {}

QStringList BaseSoftwareInitializer::enabledModules() const {
    return workflowSequence();
}

QSharedPointer<LogicRuntime> BaseSoftwareInitializer::logicRuntime() const {
    return logicRuntime_;
}

QSharedPointer<CommunicationHub> BaseSoftwareInitializer::communicationHub() const {
    return communicationHub_;
}
