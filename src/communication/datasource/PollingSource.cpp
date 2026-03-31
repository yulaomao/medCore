#include "PollingSource.h"
#include <QPointer>

PollingSource::PollingSource(PollingTask* task, QObject* parent)
    : SourceBase(task ? task->channel() : QString(), parent)
    , task_(task)
    , timer_(new QTimer(this))
{
    if (!task_) {
        return;
    }

    task_->setParent(this);
    timer_->setTimerType(Qt::PreciseTimer);
    connect(timer_, &QTimer::timeout, this, &PollingSource::onTimerTick);
    connect(task_, &PollingTask::sampleReady, this, &PollingSource::onSampleReady,
            Qt::QueuedConnection);
}

PollingSource::~PollingSource() = default;

void PollingSource::start() {
    if (!task_) return;
    const int intervalMs = task_->intervalMs();
    if (intervalMs <= 0) return;
    timer_->start(intervalMs);
}

void PollingSource::stop() {
    timer_->stop();
}

void PollingSource::onTimerTick() {
    if (!task_) return;
    if (taskRunning_->exchange(true, std::memory_order_acq_rel)) {
        return;
    }

    QPointer<PollingTask> guard(task_);
    const auto sharedRunningFlag = taskRunning_;
    QThreadPool::globalInstance()->start([guard, sharedRunningFlag]() {
        if (guard) {
            guard->run();
        }
        sharedRunningFlag->store(false, std::memory_order_release);
    });
}

void PollingSource::onSampleReady(const StateSample& sample) {
    emit sampleReady(sample);
}
