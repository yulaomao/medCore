#pragma once
#include <QObject>
#include "StateSample.h"

class SourceBase : public QObject {
    Q_OBJECT
public:
    explicit SourceBase(const QString& channelName, QObject* parent = nullptr);
    ~SourceBase() override = default;

    QString channelName() const;
    QString lastError() const;

    virtual void start() = 0;
    virtual void stop()  = 0;

signals:
    void sampleReady(const StateSample& sample);
    void errorOccurred(const QString& channelName, const QString& errorMessage);

protected:
    void reportError(const QString& errorMessage);

private:
    QString channelName_;
    QString lastError_;
};
