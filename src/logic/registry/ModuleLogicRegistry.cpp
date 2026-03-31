// 文件说明：实现模块逻辑处理器注册表，维护模块名到处理器实例的映射关系。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "ModuleLogicRegistry.h"

ModuleLogicRegistry::ModuleLogicRegistry(QObject* parent) : QObject(parent) {}

void ModuleLogicRegistry::registerHandler(QSharedPointer<ModuleLogicHandler> handler) {
    if (!handler) return;
    if (handlers_.contains(handler->moduleName())) {
        handlerList_.removeOne(handlers_.value(handler->moduleName()));
    }
    handlers_[handler->moduleName()] = handler;
    handlerList_.append(handler);
}

QSharedPointer<ModuleLogicHandler> ModuleLogicRegistry::getHandler(const QString& moduleName) const {
    return handlers_.value(moduleName);
}

const QVector<QSharedPointer<ModuleLogicHandler>>& ModuleLogicRegistry::allHandlers() const {
    return handlerList_;
}
