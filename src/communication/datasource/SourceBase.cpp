#include "SourceBase.h"

SourceBase::SourceBase(const QString& channelName, QObject* parent)
    : QObject(parent)
    , channelName_(channelName)
{}

QString SourceBase::channelName() const { return channelName_; }
