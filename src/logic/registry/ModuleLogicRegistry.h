#pragma once
#include <QObject>
#include <QHash>
#include <QVector>
#include <QSharedPointer>
#include "ModuleLogicHandler.h"

class ModuleLogicRegistry : public QObject {
    Q_OBJECT
public:
    explicit ModuleLogicRegistry(QObject* parent = nullptr);

    void registerHandler(QSharedPointer<ModuleLogicHandler> handler);
    QSharedPointer<ModuleLogicHandler> getHandler(const QString& moduleName) const;
    QVector<QSharedPointer<ModuleLogicHandler>> allHandlers() const;

private:
    QHash<QString, QSharedPointer<ModuleLogicHandler>> handlers_;
};
