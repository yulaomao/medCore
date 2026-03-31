#include "ModuleLogicRegistry.h"

ModuleLogicRegistry::ModuleLogicRegistry(QObject* parent) : QObject(parent) {}

void ModuleLogicRegistry::registerHandler(QSharedPointer<ModuleLogicHandler> handler) {
    if (!handler) return;
    if (handlers_.contains(handler->moduleName())) {
        handlerList_.removeAll(handlers_.value(handler->moduleName()));
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
