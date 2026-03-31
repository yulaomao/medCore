#pragma once
#include <QObject>
#include <QSharedPointer>
#include "../../logic/gateway/ILogicGateway.h"
#include "../../logic/registry/ModuleLogicRegistry.h"
#include "../../logic/workflow/WorkflowStateMachine.h"
#include "../../logic/scene/SceneGraph.h"

class LogicRuntime : public ILogicGateway {
    Q_OBJECT
public:
    explicit LogicRuntime(ModuleLogicRegistry* registry,
                          WorkflowStateMachine* workflow,
                          SceneGraph* sceneGraph,
                          QObject* parent = nullptr);
    ~LogicRuntime() override;

    // ILogicGateway interface
    void sendAction(const UiAction& action) override;
    void subscribeNotification(QObject* receiver, const char* slot) override;
    void unsubscribeNotification(QObject* receiver) override;
    QString getConnectionState() const override;
    void requestResync() override;

    void start();
    void stop();

signals:
    // notificationReceived is inherited from ILogicGateway

public slots:
    void processAction(const UiAction& action);
    void onStateSample(const QString& channel, const QJsonObject& data);

private slots:
    void onHandlerNotification(const LogicNotification& notification);

private:
    ModuleLogicRegistry* registry_;
    WorkflowStateMachine* workflow_;
    SceneGraph* sceneGraph_;
    QString connectionState_{"Disconnected"};
};
