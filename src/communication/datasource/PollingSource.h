#pragma once
#include "SourceBase.h"
#include "PollingTask.h"
#include <QTimer>
#include <QThreadPool>
#include <atomic>
#include <memory>

class PollingSource : public SourceBase {
    Q_OBJECT
public:
    explicit PollingSource(PollingTask* task, QObject* parent = nullptr);
    ~PollingSource() override;

    void start() override;
    void stop()  override;

private slots:
    void onTimerTick();
    void onSampleReady(const StateSample& sample);

private:
    PollingTask* task_;
    QTimer* timer_;
    std::shared_ptr<std::atomic_bool> taskRunning_{
        std::make_shared<std::atomic_bool>(false)
    };
};
