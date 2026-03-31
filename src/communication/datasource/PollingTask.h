#pragma once
#include <QObject>
#include <QJsonObject>
#include <atomic>
#include "StateSample.h"

class PollingTask : public QObject {
    Q_OBJECT
public:
    explicit PollingTask(const QString& channel, int intervalMs, QObject* parent = nullptr);
    ~PollingTask() override = default;

    QString channel() const;
    int intervalMs() const;

    virtual QJsonObject poll() const = 0;

signals:
    void sampleReady(const StateSample& sample);

public slots:
    void run();

private:
    QString channel_;
    int intervalMs_;
    std::atomic<quint64> sequenceNumber_{0};
};
