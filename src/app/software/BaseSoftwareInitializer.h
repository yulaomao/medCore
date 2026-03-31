// 文件说明：软件初始化基类声明，定义启动、关闭与模块配置接口。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QList>
#include <QObject>
#include <QSharedPointer>
#include <QStringList>

class QApplication;
class ApplicationCoordinator;
class GlobalUiManager;
class LogicRuntime;
class MainWindow;
class MessageRouter;
class ModuleCoordinator;
class ModuleLogicRegistry;
class SceneGraph;
class CommunicationHub;
class VtkSceneWindow;

struct SceneWindowRegistration {
    QString windowId;
    VtkSceneWindow* window{nullptr};
};

struct ModuleUiRegistration {
    QSharedPointer<ModuleCoordinator> coordinator;
    QList<SceneWindowRegistration> sceneWindows;
};

struct SoftwareBootstrapResult {
    MainWindow* mainWindow{nullptr};
    ApplicationCoordinator* appCoordinator{nullptr};
    QSharedPointer<LogicRuntime> logicRuntime;
    QSharedPointer<CommunicationHub> communicationHub;

    bool isValid() const;
};

/// 软件初始化基类。
///
/// 约束不同软件方案的启动与关闭流程，并提供工作流、初始模块和启用模块配置。
class BaseSoftwareInitializer : public QObject {
    Q_OBJECT
public:
    // --- 生命周期与模块配置 ---
    explicit BaseSoftwareInitializer(QObject* parent = nullptr);
    ~BaseSoftwareInitializer() override = default;

    SoftwareBootstrapResult build(QApplication& application);
    void shutdown();
    virtual QStringList workflowSequence() const = 0;
    virtual QString initialModule() const = 0;
    virtual QStringList enabledModules() const;

    // --- 运行时对象访问 ---
    QSharedPointer<LogicRuntime>      logicRuntime()      const;
    QSharedPointer<CommunicationHub>  communicationHub()  const;

signals:
    void initializationComplete();
    void initializationFailed(const QString& reason);
    void shutdownComplete();

protected:
    virtual void registerModuleHandlers(const QStringList& enabledModules,
                                        ModuleLogicRegistry* registry,
                                        SceneGraph* sceneGraph) = 0;
    virtual QList<ModuleUiRegistration> createModuleUiRegistrations(const QStringList& enabledModules,
                                                                    QObject* parent) = 0;
    virtual void configureCommunication(MessageRouter* messageRouter,
                                        CommunicationHub* communicationHub,
                                        QObject* parent);

    QSharedPointer<LogicRuntime>     logicRuntime_;      // 逻辑运行时实例。
    QSharedPointer<CommunicationHub> communicationHub_;  // 通信枢纽实例。
};
