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
    // Required for VTK + Qt OpenGL
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication app(argc, argv);
    app.setApplicationName("medCore");
    app.setApplicationVersion("1.0.0");

    // Register custom metatypes for cross-thread signal/slot
    qRegisterMetaType<UiAction>("UiAction");
    qRegisterMetaType<LogicNotification>("LogicNotification");
    qRegisterMetaType<StateSample>("StateSample");

    // 1. Resolve software type (falls back to "default" if Redis not available)
    RedisSoftwareResolver resolver;
    QString softwareType = resolver.resolve("medcore.config");

    // 2. Build core infrastructure
    auto* sceneGraph = new SceneGraph(&app);

    QStringList workflow = {"params", "pointpick", "planning", "navigation"};
    auto* workflowFSM = new WorkflowStateMachine(workflow, "params", &app);
    auto* registry    = new ModuleLogicRegistry(&app);

    // 3. Register module logic handlers
    auto paramsHandler    = QSharedPointer<ParamsModuleHandler>::create(sceneGraph);
    auto pointPickHandler = QSharedPointer<PointPickModuleHandler>::create(sceneGraph);
    auto planningHandler  = QSharedPointer<PlanningModuleHandler>::create(sceneGraph);
    auto navHandler       = QSharedPointer<NavigationModuleHandler>::create(sceneGraph);

    registry->registerHandler(paramsHandler);
    registry->registerHandler(pointPickHandler);
    registry->registerHandler(planningHandler);
    registry->registerHandler(navHandler);

    // 4. Create and start LogicRuntime
    auto logicRuntime = QSharedPointer<LogicRuntime>::create(registry, workflowFSM, sceneGraph);
    logicRuntime->start();

    // 5. CommunicationHub
    auto* messageRouter  = new MessageRouter(&app);
    auto communicationHub = QSharedPointer<CommunicationHub>::create();
    communicationHub->setMessageRouter(messageRouter);
    communicationHub->setLogicRuntime(logicRuntime.data());
    communicationHub->start();

    // 6. Create VTK scene windows
    auto* pointPickWindow = new VtkSceneWindow("pointpick-window");
    auto* planningWindow  = new VtkSceneWindow("planning-window");
    auto* navWindow       = new VtkSceneWindow("navigation-window");

    // 7. Create module coordinators
    auto paramsCoord    = QSharedPointer<ParamsModuleCoordinator>::create();
    auto pointPickCoord = QSharedPointer<PointPickModuleCoordinator>::create(pointPickWindow);
    auto planningCoord  = QSharedPointer<PlanningModuleCoordinator>::create(planningWindow);
    auto navCoord       = QSharedPointer<NavigationModuleCoordinator>::create(navWindow);

    // 8. UI managers
    auto* stackedWidget = new QStackedWidget();
    auto* pageManager   = new PageManager(stackedWidget, &app);
    auto* uiManager     = new GlobalUiManager(&app);

    uiManager->register3dWindow("pointpick-window", pointPickWindow);
    uiManager->register3dWindow("planning-window",  planningWindow);
    uiManager->register3dWindow("navigation-window", navWindow);

    // 9. ApplicationCoordinator
    auto* appCoordinator = new ApplicationCoordinator(logicRuntime.data(),
                                                        pageManager, uiManager, &app);
    appCoordinator->registerModuleCoordinator(paramsCoord);
    appCoordinator->registerModuleCoordinator(pointPickCoord);
    appCoordinator->registerModuleCoordinator(planningCoord);
    appCoordinator->registerModuleCoordinator(navCoord);

    // 10. Wire LogicRuntime notifications -> ApplicationCoordinator
    logicRuntime->subscribeNotification(appCoordinator,
        SLOT(onNotification(LogicNotification)));

    // 11. Shell
    auto* workspaceShell = new WorkspaceShell();
    workspaceShell->setPageArea(stackedWidget);

    // Toolbar navigation actions
    for (const auto& mod : workflow) {
        auto* action = new QAction(mod, workspaceShell);
        QObject::connect(action, &QAction::triggered, [appCoordinator, mod]() {
            appCoordinator->activateModule(mod);
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

    // 12. Activate initial module
    appCoordinator->activateModule(workflowFSM->currentModule());

    // 13. Show main window
    mainWindow->show();

    return app.exec();
}
