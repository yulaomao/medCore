#pragma once
#include <QObject>
#include <QSharedPointer>
#include <QStringList>

class LogicRuntime;
class CommunicationHub;

class BaseSoftwareInitializer : public QObject {
    Q_OBJECT
public:
    explicit BaseSoftwareInitializer(QObject* parent = nullptr);
    ~BaseSoftwareInitializer() override = default;

    virtual void initialize() = 0;
    virtual void shutdown()   = 0;
    virtual QStringList workflowSequence() const = 0;
    virtual QString initialModule() const = 0;
    virtual QStringList enabledModules() const;

    QSharedPointer<LogicRuntime>      logicRuntime()      const;
    QSharedPointer<CommunicationHub>  communicationHub()  const;

signals:
    void initializationComplete();
    void initializationFailed(const QString& reason);
    void shutdownComplete();

protected:
    QSharedPointer<LogicRuntime>     logicRuntime_;
    QSharedPointer<CommunicationHub> communicationHub_;
};
