#include "PollingSource.h"
#include <QPointer>

PollingSource::PollingSource(PollingTask* task, QObject* parent)
    : SourceBase(task->channel(), parent)
    , task_(task)
    , timer_(new QTimer(this))
{
    task_->setParent(this);
    timer_->setTimerType(Qt::PreciseTimer);
    connect(timer_, &QTimer::timeout, this, &PollingSource::onTimerTick);
    connect(task_, &PollingTask::sampleReady, this, &PollingSource::onSampleReady,
            Qt::QueuedConnection);
}

PollingSource::~PollingSource() = default;

void PollingSource::start() {
    const int intervalMs = task_->intervalMs();
    if (intervalMs <= 0) return;
    timer_->start(intervalMs);
}

void PollingSource::stop() {
    timer_->stop();
}

void PollingSource::onTimerTick() {
    if (taskRunning_.exchange(true, std::memory_order_acq_rel)) {
        return;
    }

    QPointer<PollingTask> guard(task_);
    QPointer<PollingSource> sourceGuard(this);
    QThreadPool::globalInstance()->start([guard, sourceGuard]() {
        if (guard) {
            guard->run();
        }
        if (sourceGuard) {
            sourceGuard->taskRunning_.store(false, std::memory_order_release);
        }
    });
}

void PollingSource::onSampleReady(const StateSample& sample) {
    emit sampleReady(sample);
}
