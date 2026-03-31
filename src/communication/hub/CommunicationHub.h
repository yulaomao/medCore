#pragma once
#include <QObject>
#include <QHash>
#include <QSharedPointer>
#include "../datasource/SourceBase.h"
#include "../routing/MessageRouter.h"

// Forward declare to avoid circular dependency with LogicRuntime
class LogicRuntime;

class CommunicationHub : public QObject {
    Q_OBJECT
public:
    explicit CommunicationHub(QObject* parent = nullptr);
    ~CommunicationHub() override = default;

    void addDataSource(QSharedPointer<SourceBase> source);
    void removeDataSource(const QString& channelName);

    void setMessageRouter(MessageRouter* router);
    void setLogicRuntime(LogicRuntime* runtime);

    void start();
    void stop();

signals:
    void stateSampleReceived(const StateSample& sample);
    void dataSourceError(const QString& channelName, const QString& errorMessage);

private slots:
    void onSampleReady(const StateSample& sample);
    void onSourceError(const QString& channelName, const QString& errorMessage);

private:
    QHash<QString, QSharedPointer<SourceBase>> sources_;
    MessageRouter* router_{nullptr};
    LogicRuntime* logicRuntime_{nullptr};
};
