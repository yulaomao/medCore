#pragma once
#include "SourceBase.h"
#include <QTimer>

class SubscriptionSource : public SourceBase {
    Q_OBJECT
public:
    explicit SubscriptionSource(const QString& channelName,
                                 int intervalMs = 1000,
                                 QObject* parent = nullptr);
    ~SubscriptionSource() override = default;

    void start() override;
    void stop()  override;

private slots:
    void onTimer();

private:
    QTimer* timer_;
    int intervalMs_;
    quint64 sequenceNumber_{0};
};
