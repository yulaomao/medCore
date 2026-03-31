#include "BaseSoftwareInitializer.h"
#include "../../logic/runtime/LogicRuntime.h"
#include "../../communication/hub/CommunicationHub.h"

BaseSoftwareInitializer::BaseSoftwareInitializer(QObject* parent) : QObject(parent) {}

QSharedPointer<LogicRuntime> BaseSoftwareInitializer::logicRuntime() const {
    return logicRuntime_;
}

QSharedPointer<CommunicationHub> BaseSoftwareInitializer::communicationHub() const {
    return communicationHub_;
}
