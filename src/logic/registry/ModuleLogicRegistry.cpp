#include "ModuleLogicRegistry.h"

ModuleLogicRegistry::ModuleLogicRegistry(QObject* parent) : QObject(parent) {}

void ModuleLogicRegistry::registerHandler(QSharedPointer<ModuleLogicHandler> handler) {
    if (!handler) return;
    handlers_[handler->moduleName()] = handler;
}

QSharedPointer<ModuleLogicHandler> ModuleLogicRegistry::getHandler(const QString& moduleName) const {
    return handlers_.value(moduleName);
}

QVector<QSharedPointer<ModuleLogicHandler>> ModuleLogicRegistry::allHandlers() const {
    return QVector<QSharedPointer<ModuleLogicHandler>>(handlers_.values().begin(), handlers_.values().end());
}
