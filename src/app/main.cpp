// 文件说明：应用程序入口，负责组装场景、工作流、通信、界面协调器与主窗口。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include <QApplication>
#include <QSurfaceFormat>
#include <QSharedPointer>
#include <QDebug>

#include <QVTKOpenGLNativeWidget.h>

#include "../contracts/UiAction.h"
#include "../contracts/LogicNotification.h"
#include "../logic/scene/SceneGraph.h"
#include "../logic/registry/ModuleLogicRegistry.h"
#include "../logic/workflow/WorkflowStateMachine.h"
#include "../logic/runtime/LogicRuntime.h"
#include "../communication/hub/CommunicationHub.h"
#include "../communication/routing/MessageRouter.h"
#include "../ui/vtk3d/VtkSceneWindow.h"
#include "../ui/pages/PageManager.h"
#include "../ui/globalui/GlobalUiManager.h"
#include "../ui/coordination/ApplicationCoordinator.h"
#include "../shell/WorkspaceShell.h"
#include "../shell/MainWindow.h"
#include "../app/software/RedisSoftwareResolver.h"
#include "../app/software/SoftwareInitializerFactory.h"
#include "../modules/params/ParamsModuleHandler.h"
#include "../modules/params/ParamsModuleCoordinator.h"
#include "../modules/pointpick/PointPickModuleHandler.h"
#include "../modules/pointpick/PointPickModuleCoordinator.h"
#include "../modules/planning/PlanningModuleHandler.h"
#include "../modules/planning/PlanningModuleCoordinator.h"
#include "../modules/navigation/NavigationModuleHandler.h"
#include "../modules/navigation/NavigationModuleCoordinator.h"
#include <QStackedWidget>
#include <QPointer>

int main(int argc, char* argv[]) {
    // 为 VTK 与 Qt 的 OpenGL 集成设置默认渲染格式
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication app(argc, argv);
    app.setApplicationName("medCore");
    app.setApplicationVersion("1.0.0");

    // 注册跨线程信号槽所需的自定义元类型
    qRegisterMetaType<UiAction>("UiAction");
    qRegisterMetaType<LogicNotification>("LogicNotification");
    qRegisterMetaType<StateSample>("StateSample");

    // 1. 解析软件类型；若 Redis 不可用则回退到默认配置
    RedisSoftwareResolver resolver;
    QString softwareType = resolver.resolve("medcore.config");

    auto softwareInitializer = SoftwareInitializerFactory::create(softwareType);
    const QStringList workflow = softwareInitializer
        ? softwareInitializer->workflowSequence()
        : QStringList{"params", "pointpick", "planning", "navigation"};
    const QStringList enabledModules = softwareInitializer
        ? softwareInitializer->enabledModules()
        : workflow;
    const QString initialModule = softwareInitializer
        ? softwareInitializer->initialModule()
        : QStringLiteral("params");

    // 2. 构建核心基础设施对象
    auto* sceneGraph = new SceneGraph(&app);
    auto* workflowFSM = new WorkflowStateMachine(workflow, initialModule, &app);
    workflowFSM->setEnterableModules(enabledModules);
    auto* registry    = new ModuleLogicRegistry(&app);

    // 3. 按启用模块注册逻辑处理器
    if (enabledModules.contains("params"))
        registry->registerHandler(QSharedPointer<ParamsModuleHandler>::create(sceneGraph));
    if (enabledModules.contains("pointpick"))
        registry->registerHandler(QSharedPointer<PointPickModuleHandler>::create(sceneGraph));
    if (enabledModules.contains("planning"))
        registry->registerHandler(QSharedPointer<PlanningModuleHandler>::create(sceneGraph));
    if (enabledModules.contains("navigation"))
        registry->registerHandler(QSharedPointer<NavigationModuleHandler>::create(sceneGraph));

    // 4. 创建并启动逻辑运行时
    auto logicRuntime = QSharedPointer<LogicRuntime>::create(registry, workflowFSM, sceneGraph);
    logicRuntime->start();

    // 5. 创建通信枢纽并连接消息路由
    auto* messageRouter  = new MessageRouter(&app);
    auto communicationHub = QSharedPointer<CommunicationHub>::create();
    communicationHub->setMessageRouter(messageRouter);
    communicationHub->setLogicRuntime(logicRuntime.data());
    communicationHub->start();

    // 6. 创建各模块三维场景窗口，并交给应用对象管理生命周期
    auto* pointPickWindow = new VtkSceneWindow("pointpick-window");
    auto* planningWindow  = new VtkSceneWindow("planning-window");
    auto* navWindow       = new VtkSceneWindow("navigation-window");

    // 7. 创建各模块界面协调器
    auto paramsCoord    = enabledModules.contains("params")
        ? QSharedPointer<ParamsModuleCoordinator>::create() : QSharedPointer<ParamsModuleCoordinator>();
    auto pointPickCoord = enabledModules.contains("pointpick")
        ? QSharedPointer<PointPickModuleCoordinator>::create(pointPickWindow) : QSharedPointer<PointPickModuleCoordinator>();
    auto planningCoord  = enabledModules.contains("planning")
        ? QSharedPointer<PlanningModuleCoordinator>::create(planningWindow) : QSharedPointer<PlanningModuleCoordinator>();
    auto navCoord       = enabledModules.contains("navigation")
        ? QSharedPointer<NavigationModuleCoordinator>::create(navWindow) : QSharedPointer<NavigationModuleCoordinator>();

    // 8. 创建界面管理对象，页面容器后续会移交给 WorkspaceShell 托管
    auto* stackedWidget = new QStackedWidget();
    auto* pageManager   = new PageManager(stackedWidget, &app);
    auto* uiManager     = new GlobalUiManager(&app);

    if (enabledModules.contains("pointpick"))
        uiManager->register3dWindow("pointpick-window", pointPickWindow);
    if (enabledModules.contains("planning"))
        uiManager->register3dWindow("planning-window", planningWindow);
    if (enabledModules.contains("navigation"))
        uiManager->register3dWindow("navigation-window", navWindow);

    // 9. 创建应用级协调器
    auto* appCoordinator = new ApplicationCoordinator(logicRuntime.data(),
                                                        pageManager, uiManager, &app);
    if (paramsCoord) appCoordinator->registerModuleCoordinator(paramsCoord);
    if (pointPickCoord) appCoordinator->registerModuleCoordinator(pointPickCoord);
    if (planningCoord) appCoordinator->registerModuleCoordinator(planningCoord);
    if (navCoord) appCoordinator->registerModuleCoordinator(navCoord);

    // 10. 将逻辑运行时通知连接到应用协调器
    logicRuntime->subscribeNotification(appCoordinator,
        SLOT(onNotification(LogicNotification)));

    // 11. 创建工作区外壳与主窗口
    auto* workspaceShell = new WorkspaceShell();
    workspaceShell->setPageArea(stackedWidget);

    // 为工作流中的每个模块生成工具栏导航动作
    for (const auto& mod : workflow) {
        auto* action = new QAction(mod, workspaceShell);
        QObject::connect(action, &QAction::triggered, [appCoordinator, mod]() {
            appCoordinator->requestModuleActivation(mod);
        });
        workspaceShell->addToolbarAction(action);
    }

    auto* mainWindow = new MainWindow(workspaceShell);
    appCoordinator->connectShellSignals(mainWindow);

    QPointer<CommunicationHub> communicationHubGuard(communicationHub.data());
    QPointer<LogicRuntime> logicRuntimeGuard(logicRuntime.data());
    QObject::connect(&app, &QCoreApplication::aboutToQuit,
                     [communicationHubGuard, logicRuntimeGuard]() {
        if (communicationHubGuard) communicationHubGuard->stop();
        if (logicRuntimeGuard) logicRuntimeGuard->stop();
    });

    // 12. 激活初始模块
    appCoordinator->activateModule(workflowFSM->currentModule());

    // 13. 显示主窗口并进入事件循环
    mainWindow->show();

    return app.exec();
}
