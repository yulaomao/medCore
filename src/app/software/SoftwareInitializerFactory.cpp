// 文件说明：实现软件初始化器工厂，按软件类型返回对应的初始化配置。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "SoftwareInitializerFactory.h"
#include "../../logic/registry/ModuleLogicRegistry.h"
#include "../../logic/scene/SceneGraph.h"
#include "../../modules/navigation/NavigationModuleCoordinator.h"
#include "../../modules/navigation/NavigationModuleHandler.h"
#include "../../modules/params/ParamsModuleCoordinator.h"
#include "../../modules/params/ParamsModuleHandler.h"
#include "../../modules/planning/PlanningModuleCoordinator.h"
#include "../../modules/planning/PlanningModuleHandler.h"
#include "../../modules/pointpick/PointPickModuleCoordinator.h"
#include "../../modules/pointpick/PointPickModuleHandler.h"
#include "../../ui/vtk3d/VtkSceneWindow.h"
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

    void registerModuleHandlers(const QStringList& enabledModules,
                                ModuleLogicRegistry* registry,
                                SceneGraph* sceneGraph) override {
        if (!registry || !sceneGraph) return;

        if (enabledModules.contains("params"))
            registry->registerHandler(QSharedPointer<ParamsModuleHandler>::create(sceneGraph));
        if (enabledModules.contains("pointpick"))
            registry->registerHandler(QSharedPointer<PointPickModuleHandler>::create(sceneGraph));
        if (enabledModules.contains("planning"))
            registry->registerHandler(QSharedPointer<PlanningModuleHandler>::create(sceneGraph));
        if (enabledModules.contains("navigation"))
            registry->registerHandler(QSharedPointer<NavigationModuleHandler>::create(sceneGraph));
    }

    QList<ModuleUiRegistration> createModuleUiRegistrations(const QStringList& enabledModules,
                                                            QObject* parent) override {
        QList<ModuleUiRegistration> registrations;

        if (enabledModules.contains("params")) {
            ModuleUiRegistration registration;
            registration.coordinator = QSharedPointer<ParamsModuleCoordinator>::create(parent);
            registrations.append(registration);
        }

        if (enabledModules.contains("pointpick")) {
            auto* window = new VtkSceneWindow("pointpick-window");
            ModuleUiRegistration registration;
            registration.coordinator = QSharedPointer<PointPickModuleCoordinator>::create(window, parent);
            registration.sceneWindows.append({QStringLiteral("pointpick-window"), window});
            registrations.append(registration);
        }

        if (enabledModules.contains("planning")) {
            auto* window = new VtkSceneWindow("planning-window");
            ModuleUiRegistration registration;
            registration.coordinator = QSharedPointer<PlanningModuleCoordinator>::create(window, parent);
            registration.sceneWindows.append({QStringLiteral("planning-window"), window});
            registrations.append(registration);
        }

        if (enabledModules.contains("navigation")) {
            auto* window = new VtkSceneWindow("navigation-window");
            ModuleUiRegistration registration;
            registration.coordinator = QSharedPointer<NavigationModuleCoordinator>::create(window, parent);
            registration.sceneWindows.append({QStringLiteral("navigation-window"), window});
            registrations.append(registration);
        }

        return registrations;
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
