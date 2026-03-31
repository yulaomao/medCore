// 文件说明：轮询式数据源声明，负责按周期触发采样任务并转发结果。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include "SourceBase.h"
#include "PollingTask.h"
#include <QTimer>
#include <QThreadPool>
#include <atomic>
#include <memory>

/// 轮询式数据源。
///
/// 按固定周期触发 PollingTask，并将异步采样结果转发为 SourceBase 信号。
class PollingSource : public SourceBase {
    Q_OBJECT
public:
    // --- 生命周期 ---
    explicit PollingSource(PollingTask* task, QObject* parent = nullptr);
    ~PollingSource() override;

    void start() override;
    void stop()  override;

private slots:
    void onTimerTick();
    void onSampleReady(const StateSample& sample);

private:
    PollingTask* task_;   // 被调度的轮询任务。
    QTimer* timer_;       // 触发轮询执行的定时器。
    std::shared_ptr<std::atomic_bool> taskRunning_{
        std::make_shared<std::atomic_bool>(false)
    };                    // 标记当前是否已有任务在执行，避免重复并发。
};
