#pragma once
#include <QObject>
#include <QSharedPointer>

class LogicRuntime;
class CommunicationHub;

class BaseSoftwareInitializer : public QObject {
    Q_OBJECT
public:
    explicit BaseSoftwareInitializer(QObject* parent = nullptr);
    ~BaseSoftwareInitializer() override = default;

    virtual void initialize() = 0;
    virtual void shutdown()   = 0;

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
