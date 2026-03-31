// 文件说明：轮询任务声明，封装单个通道的采样配置与执行入口。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QObject>
#include <QJsonObject>
#include <QUuid>
#include <atomic>
#include "StateSample.h"

/// 轮询任务抽象。
///
/// 封装单个采样任务的执行配置、节奏控制信息以及通道元数据。
class PollingTask : public QObject {
    Q_OBJECT
public:
    // --- 任务基础信息与调度字段（设计 19.6） ---
    explicit PollingTask(const QString& channel, int intervalMs, QObject* parent = nullptr);
    ~PollingTask() override = default;

    QUuid taskId() const;
    QString channel() const;
    int intervalMs() const;
    QString module() const;
    void setModule(const QString& module);
    QString redisKey() const;
    void setRedisKey(const QString& key);
    QString mergeStrategy() const;
    void setMergeStrategy(const QString& strategy);
    QString changeDetection() const;
    void setChangeDetection(const QString& detection);
    int priority() const;
    void setPriority(int priority);
    double maxDispatchRateHz() const;
    void setMaxDispatchRateHz(double rateHz);

    // 由具体任务实现实际采样逻辑。
    virtual QJsonObject poll() const = 0;

signals:
    void sampleReady(const StateSample& sample);

public slots:
    void run();

private:
    QUuid taskId_;                              // 任务唯一标识。
    QString channel_;                           // 样本输出通道。
    int intervalMs_;                            // 理想轮询周期，单位毫秒。
    QString module_;                            // 任务所属模块。
    QString redisKey_;                          // 若使用 Redis，则对应的键名。
    QString mergeStrategy_{"latest-wins"};     // 采样结果合并策略。
    QString changeDetection_;                   // 变化检测策略名称。
    int priority_{0};                           // 调度优先级。
    double maxDispatchRateHz_{0.0};             // 结果分发频率上限。
    std::atomic<quint64> sequenceNumber_{0};    // 本任务递增样本序号。
};
