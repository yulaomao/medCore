#include "SourceBase.h"

SourceBase::SourceBase(const QString& channelName, QObject* parent)
    : QObject(parent)
    , channelName_(channelName)
{}

QString SourceBase::channelName() const { return channelName_; }

QString SourceBase::lastError() const { return lastError_; }

void SourceBase::reportError(const QString& errorMessage) {
    lastError_ = errorMessage;
    emit errorOccurred(channelName_, errorMessage);
}
