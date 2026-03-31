#pragma once
#include <QSharedPointer>
#include <QString>
#include "BaseSoftwareInitializer.h"

class SoftwareInitializerFactory {
public:
    static QSharedPointer<BaseSoftwareInitializer> create(const QString& softwareType);

private:
    SoftwareInitializerFactory() = delete;
};
