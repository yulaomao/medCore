#pragma once
#include <QObject>
#include <QStringList>

class WorkflowStateMachine : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentModule READ currentModule NOTIFY moduleChanged)
public:
    explicit WorkflowStateMachine(const QStringList& workflowSequence,
                                   const QString& initialModule,
                                   QObject* parent = nullptr);

    QString currentModule() const;
    QStringList workflowSequence() const;
    bool canEnter(const QString& module) const;
    bool advance(const QString& targetModule);

    void setEnterableModules(const QStringList& modules);
    QStringList enterableModules() const;

    void reset();

signals:
    void moduleChanged(const QString& oldModule, const QString& newModule);

private:
    QStringList workflowSequence_;
    QString initialModule_;
    QString currentModule_;
    QStringList enterableModules_;
};
