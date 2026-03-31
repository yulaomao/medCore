cmake_minimum_required(VERSION 3.16)
project(medCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Network)

set(MEDCORE_QT_MODULES
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

# VTK pre-compiled installation directory
set(MEDCORE_VTK_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirtyPart/vtk")
if (NOT EXISTS "${MEDCORE_VTK_INSTALL_DIR}")
    message(FATAL_ERROR "VTK pre-built not found at ${MEDCORE_VTK_INSTALL_DIR}. Please compile VTK first using: build_vtk.ps1 or cmake commands provided in the build notes.")
endif()

list(APPEND CMAKE_PREFIX_PATH "${MEDCORE_VTK_INSTALL_DIR}")
find_package(VTK REQUIRED
    COMPONENTS
        CommonColor
        CommonCore
        FiltersSources
        GUISupportQt
        InteractionStyle
        RenderingCore
        RenderingOpenGL2
)

set(MEDCORE_VTK_MODULES
    VTK::CommonColor
    VTK::CommonCore
    VTK::FiltersSources
    VTK::GUISupportQt
    VTK::InteractionStyle
    VTK::RenderingCore
    VTK::RenderingOpenGL2
)

set(SOURCES
    src/app/main.cpp

    # Contracts (header-only structs, no .cpp needed beyond metatype registration)

    # Logic gateway (header-only interface)
    src/logic/gateway/ILogicGateway.h

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
    MODULES ${MEDCORE_VTK_MODULES}
)

target_link_libraries(medCore PRIVATE
    ${MEDCORE_QT_MODULES}
    ${MEDCORE_VTK_MODULES}
)

if (WIN32 AND QT_VERSION_MAJOR EQUAL 6)
    get_target_property(MEDCORE_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    if (MEDCORE_QMAKE_EXECUTABLE)
        get_filename_component(MEDCORE_QT_BIN_DIR "${MEDCORE_QMAKE_EXECUTABLE}" DIRECTORY)
        find_program(MEDCORE_WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${MEDCORE_QT_BIN_DIR}")
        if (MEDCORE_WINDEPLOYQT_EXECUTABLE)
            add_custom_command(TARGET medCore POST_BUILD
                COMMAND "${MEDCORE_WINDEPLOYQT_EXECUTABLE}"
                    "$<$<CONFIG:Debug>:--debug>$<$<NOT:$<CONFIG:Debug>>:--release>"
                    --no-translations
                    --no-system-d3d-compiler
                    --no-opengl-sw
                    "$<TARGET_FILE:medCore>"
                COMMENT "Deploying Qt runtime dependencies via windeployqt"
                VERBATIM
            )
        else()
            message(WARNING "windeployqt not found; medCore may fail to run without Qt runtime DLLs.")
        endif()
    else()
        message(WARNING "Qt6::qmake location unavailable; skipping automatic Qt runtime deployment.")
    endif()
endif()

if (WIN32)
    add_custom_command(TARGET medCore POST_BUILD
        COMMAND powershell -NoProfile -ExecutionPolicy Bypass -Command
            "Get-ChildItem -Path '${MEDCORE_VTK_INSTALL_DIR}/bin' -Filter '*.dll' | Copy-Item -Destination '$<TARGET_FILE_DIR:medCore>' -Force"
        COMMENT "Deploying VTK runtime dependencies"
        VERBATIM
    )
endif()
