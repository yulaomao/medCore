#include "SubscriptionSource.h"
#include <QDateTime>

SubscriptionSource::SubscriptionSource(const QString& channelName,
                                         int intervalMs,
                                         QObject* parent)
    : SourceBase(channelName, parent)
    , timer_(new QTimer(this))
    , intervalMs_(intervalMs)
{
    connect(timer_, &QTimer::timeout, this, &SubscriptionSource::onTimer);
}

void SubscriptionSource::start() {
    timer_->start(intervalMs_);
}

void SubscriptionSource::stop() {
    timer_->stop();
}

void SubscriptionSource::onTimer() {
    StateSample sample;
    sample.channel        = channelName();
    sample.data           = QJsonObject();
    sample.timestampMs    = QDateTime::currentMSecsSinceEpoch();
    sample.sequenceNumber = ++sequenceNumber_;
    emit sampleReady(sample);
}
