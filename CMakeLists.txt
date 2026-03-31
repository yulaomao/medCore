cmake_minimum_required(VERSION 3.16)
project(medCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt5 / Qt6
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Network)

# VTK
find_package(VTK REQUIRED COMPONENTS
    CommonColor
    CommonCore
    FiltersSources
    GUISupportQt
    InteractionStyle
    RenderingCore
    RenderingOpenGL2
)

set(SOURCES
    src/app/main.cpp

    # Contracts (header-only structs, no .cpp needed beyond metatype registration)

    # Logic gateway (header-only interface)

    # Scene nodes
    src/logic/scene/nodes/NodeBase.cpp
    src/logic/scene/nodes/PointNode.cpp
    src/logic/scene/nodes/LineNode.cpp
    src/logic/scene/nodes/ModelNode.cpp
    src/logic/scene/nodes/TransformNode.cpp

    # Scene graph
    src/logic/scene/SceneGraph.cpp

    # Logic registry
    src/logic/registry/ModuleLogicHandler.cpp
    src/logic/registry/ModuleLogicRegistry.cpp

    # Workflow
    src/logic/workflow/WorkflowStateMachine.cpp

    # Runtime
    src/logic/runtime/LogicRuntime.cpp

    # Communication
    src/communication/datasource/PollingTask.cpp
    src/communication/datasource/SourceBase.cpp
    src/communication/datasource/SubscriptionSource.cpp
    src/communication/datasource/PollingSource.cpp
    src/communication/redis/RedisGateway.cpp
    src/communication/routing/MessageRouter.cpp
    src/communication/hub/CommunicationHub.cpp

    # UI
    src/ui/vtk3d/VtkSceneWindow.cpp
    src/ui/pages/PageManager.cpp
    src/ui/globalui/GlobalUiManager.cpp
    src/ui/coordination/ModuleCoordinator.cpp
    src/ui/coordination/ApplicationCoordinator.cpp

    # Shell
    src/shell/WorkspaceShell.cpp
    src/shell/MainWindow.cpp

    # App software
    src/app/software/BaseSoftwareInitializer.cpp
    src/app/software/SoftwareInitializerFactory.cpp
    src/app/software/RedisSoftwareResolver.cpp

    # Modules
    src/modules/params/ParamsModuleHandler.cpp
    src/modules/params/ParamsModuleCoordinator.cpp
    src/modules/pointpick/PointPickModuleHandler.cpp
    src/modules/pointpick/PointPickModuleCoordinator.cpp
    src/modules/planning/PlanningModuleHandler.cpp
    src/modules/planning/PlanningModuleCoordinator.cpp
    src/modules/navigation/NavigationModuleHandler.cpp
    src/modules/navigation/NavigationModuleCoordinator.cpp
)

add_executable(medCore ${SOURCES})

set_target_properties(medCore PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

target_include_directories(medCore PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

vtk_module_autoinit(
    TARGETS medCore
    MODULES ${VTK_LIBRARIES}
)

target_link_libraries(medCore PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    ${VTK_LIBRARIES}
)
