// 文件说明：实现软件初始化器工厂，按软件类型返回对应的初始化配置。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "SoftwareInitializerFactory.h"
#include "../../logic/registry/ModuleLogicRegistry.h"
#include "../../logic/workflow/WorkflowStateMachine.h"
#include "../../logic/scene/SceneGraph.h"
#include "../../logic/runtime/LogicRuntime.h"
#include "../../communication/hub/CommunicationHub.h"
#include <QDebug>

// 具体初始化器实现
class DefaultSoftwareInitializer : public BaseSoftwareInitializer {
    Q_OBJECT
public:
    explicit DefaultSoftwareInitializer(QObject* parent = nullptr)
        : BaseSoftwareInitializer(parent) {}

    QStringList workflowSequence() const override {
        return {"params", "pointpick", "planning", "navigation"};
    }

    QString initialModule() const override {
        return "params";
    }

    void initialize() override {
        try {
            auto* registry  = new ModuleLogicRegistry();
            auto* sceneGraph = new SceneGraph();
            auto* workflow   = new WorkflowStateMachine(
                workflowSequence(), initialModule());
            workflow->setEnterableModules(enabledModules());
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

// 仅参数模块变体，当前桩实现与默认配置一致
class ParamsOnlySoftwareInitializer : public DefaultSoftwareInitializer {
    Q_OBJECT
public:
    explicit ParamsOnlySoftwareInitializer(QObject* parent = nullptr)
        : DefaultSoftwareInitializer(parent) {}

    QStringList workflowSequence() const override {
        return {"params"};
    }
};

// 全功能模块变体
class FullSoftwareInitializer : public DefaultSoftwareInitializer {
    Q_OBJECT
public:
    explicit FullSoftwareInitializer(QObject* parent = nullptr)
        : DefaultSoftwareInitializer(parent) {}

    QStringList workflowSequence() const override {
        return {"params", "pointpick", "planning", "navigation"};
    }
};

QSharedPointer<BaseSoftwareInitializer> SoftwareInitializerFactory::create(const QString& softwareType) {
    if (softwareType == "params-only")
        return QSharedPointer<ParamsOnlySoftwareInitializer>::create();
    if (softwareType == "full")
        return QSharedPointer<FullSoftwareInitializer>::create();
    // 默认配置
    return QSharedPointer<DefaultSoftwareInitializer>::create();
}

#include "SoftwareInitializerFactory.moc"
