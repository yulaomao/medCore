#include "SoftwareInitializerFactory.h"
#include "../../logic/registry/ModuleLogicRegistry.h"
#include "../../logic/workflow/WorkflowStateMachine.h"
#include "../../logic/scene/SceneGraph.h"
#include "../../logic/runtime/LogicRuntime.h"
#include "../../communication/hub/CommunicationHub.h"
#include <QDebug>

// Concrete initializer implementations
class DefaultSoftwareInitializer : public BaseSoftwareInitializer {
    Q_OBJECT
public:
    explicit DefaultSoftwareInitializer(QObject* parent = nullptr)
        : BaseSoftwareInitializer(parent) {}

    void initialize() override {
        try {
            auto* registry  = new ModuleLogicRegistry();
            auto* sceneGraph = new SceneGraph();
            auto* workflow   = new WorkflowStateMachine(
                {"params", "pointpick", "planning", "navigation"}, "params");
            logicRuntime_    = QSharedPointer<LogicRuntime>::create(registry, workflow, sceneGraph);
            communicationHub_ = QSharedPointer<CommunicationHub>::create();
            communicationHub_->setLogicRuntime(logicRuntime_.data());
            emit initializationComplete();
        } catch (const std::exception& e) {
            emit initializationFailed(QString::fromStdString(e.what()));
        }
    }

    void shutdown() override {
        if (logicRuntime_)     logicRuntime_->stop();
        if (communicationHub_) communicationHub_->stop();
        emit shutdownComplete();
    }
};

// params-only variant (same as default for stub)
class ParamsOnlySoftwareInitializer : public DefaultSoftwareInitializer {
    Q_OBJECT
public:
    explicit ParamsOnlySoftwareInitializer(QObject* parent = nullptr)
        : DefaultSoftwareInitializer(parent) {}
};

// full variant
class FullSoftwareInitializer : public DefaultSoftwareInitializer {
    Q_OBJECT
public:
    explicit FullSoftwareInitializer(QObject* parent = nullptr)
        : DefaultSoftwareInitializer(parent) {}
};

#include "SoftwareInitializerFactory.moc"

QSharedPointer<BaseSoftwareInitializer> SoftwareInitializerFactory::create(const QString& softwareType) {
    if (softwareType == "params-only")
        return QSharedPointer<ParamsOnlySoftwareInitializer>::create();
    if (softwareType == "full")
        return QSharedPointer<FullSoftwareInitializer>::create();
    // default
    return QSharedPointer<DefaultSoftwareInitializer>::create();
}
