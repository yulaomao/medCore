// 文件说明：实现订阅式数据源的启动、停止与采样推送逻辑。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

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
