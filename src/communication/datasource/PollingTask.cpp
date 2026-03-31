#include "PollingTask.h"
#include <QDateTime>

PollingTask::PollingTask(const QString& channel, int intervalMs, QObject* parent)
    : QObject(parent)
    , channel_(channel)
    , intervalMs_(intervalMs)
{}

QString PollingTask::channel() const { return channel_; }
int PollingTask::intervalMs() const { return intervalMs_; }

void PollingTask::run() {
    QJsonObject data = poll();
    StateSample sample;
    sample.channel        = channel_;
    sample.data           = data;
    sample.timestampMs    = QDateTime::currentMSecsSinceEpoch();
    sample.sequenceNumber = sequenceNumber_.fetch_add(1, std::memory_order_relaxed) + 1;
    emit sampleReady(sample);
}
