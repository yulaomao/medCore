// 文件说明：实现轮询任务的数据结构与采样构造辅助逻辑。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "PollingTask.h"
#include <QDateTime>

PollingTask::PollingTask(const QString& channel, int intervalMs, QObject* parent)
    : QObject(parent)
    , taskId_(QUuid::createUuid())
    , channel_(channel)
    , intervalMs_(intervalMs)
{}

QUuid PollingTask::taskId() const { return taskId_; }
QString PollingTask::channel() const { return channel_; }
int PollingTask::intervalMs() const { return intervalMs_; }

QString PollingTask::module() const { return module_; }
void PollingTask::setModule(const QString& module) { module_ = module; }

QString PollingTask::redisKey() const { return redisKey_; }
void PollingTask::setRedisKey(const QString& key) { redisKey_ = key; }

QString PollingTask::mergeStrategy() const { return mergeStrategy_; }
void PollingTask::setMergeStrategy(const QString& strategy) { mergeStrategy_ = strategy; }

QString PollingTask::changeDetection() const { return changeDetection_; }
void PollingTask::setChangeDetection(const QString& detection) { changeDetection_ = detection; }

int PollingTask::priority() const { return priority_; }
void PollingTask::setPriority(int priority) { priority_ = priority; }

double PollingTask::maxDispatchRateHz() const { return maxDispatchRateHz_; }
void PollingTask::setMaxDispatchRateHz(double rateHz) { maxDispatchRateHz_ = rateHz; }

void PollingTask::run() {
    QJsonObject data = poll();
    StateSample sample;
    sample.channel        = channel_;
    sample.data           = data;
    sample.timestampMs    = QDateTime::currentMSecsSinceEpoch();
    sample.sequenceNumber = sequenceNumber_.fetch_add(1, std::memory_order_relaxed) + 1;
    emit sampleReady(sample);
}
