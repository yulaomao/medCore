#pragma once
#include <QObject>
#include <QJsonObject>
#include <QUuid>
#include <atomic>
#include "StateSample.h"

class PollingTask : public QObject {
    Q_OBJECT
public:
    explicit PollingTask(const QString& channel, int intervalMs, QObject* parent = nullptr);
    ~PollingTask() override = default;

    // --- Design fields (section 19.6) ---
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

    virtual QJsonObject poll() const = 0;

signals:
    void sampleReady(const StateSample& sample);

public slots:
    void run();

private:
    QUuid taskId_;
    QString channel_;
    int intervalMs_;
    QString module_;
    QString redisKey_;
    QString mergeStrategy_{"latest-wins"};
    QString changeDetection_;
    int priority_{0};
    double maxDispatchRateHz_{0.0};
    std::atomic<quint64> sequenceNumber_{0};
};
