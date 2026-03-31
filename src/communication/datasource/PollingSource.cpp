#include "PollingSource.h"
#include <QPointer>

PollingSource::PollingSource(PollingTask* task, QObject* parent)
    : SourceBase(task->channel(), parent)
    , task_(task)
    , timer_(new QTimer(this))
{
    task_->setParent(this);
    connect(timer_, &QTimer::timeout, this, &PollingSource::onTimerTick);
    connect(task_, &PollingTask::sampleReady, this, &PollingSource::onSampleReady,
            Qt::QueuedConnection);
}

PollingSource::~PollingSource() = default;

void PollingSource::start() {
    timer_->start(task_->intervalMs());
}

void PollingSource::stop() {
    timer_->stop();
}

void PollingSource::onTimerTick() {
    QPointer<PollingTask> guard(task_);
    QThreadPool::globalInstance()->start([guard]() {
        if (guard) guard->run();
    });
}

void PollingSource::onSampleReady(const StateSample& sample) {
    emit sampleReady(sample);
}
