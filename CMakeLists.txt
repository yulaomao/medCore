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

# VTK from bundled source tree
set(MEDCORE_VTK_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirtyPart/vtk-src")
if (NOT EXISTS "${MEDCORE_VTK_SOURCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "Bundled VTK source tree not found: ${MEDCORE_VTK_SOURCE_DIR}")
endif()

set(VTK_QT_VERSION ${QT_VERSION_MAJOR} CACHE STRING "Qt major version for bundled VTK" FORCE)
set(VTK_BUILD_TESTING OFF CACHE STRING "Disable VTK tests" FORCE)
set(VTK_BUILD_EXAMPLES OFF CACHE BOOL "Disable VTK examples" FORCE)
set(VTK_BUILD_DOCUMENTATION OFF CACHE BOOL "Disable VTK documentation" FORCE)
set(VTK_BUILD_ALL_MODULES OFF CACHE BOOL "Build only requested VTK modules" FORCE)
set(VTK_ENABLE_REMOTE_MODULES OFF CACHE BOOL "Disable VTK remote modules" FORCE)
set(VTK_ENABLE_WRAPPING OFF CACHE BOOL "Disable VTK wrapping" FORCE)
set(VTK_GROUP_ENABLE_Qt YES CACHE STRING "Enable VTK Qt support" FORCE)

set(VTK_MODULE_ENABLE_VTK_CommonColor YES CACHE STRING "Enable VTK CommonColor" FORCE)
set(VTK_MODULE_ENABLE_VTK_CommonCore YES CACHE STRING "Enable VTK CommonCore" FORCE)
set(VTK_MODULE_ENABLE_VTK_FiltersSources YES CACHE STRING "Enable VTK FiltersSources" FORCE)
set(VTK_MODULE_ENABLE_VTK_GUISupportQt YES CACHE STRING "Enable VTK GUISupportQt" FORCE)
set(VTK_MODULE_ENABLE_VTK_InteractionStyle YES CACHE STRING "Enable VTK InteractionStyle" FORCE)
set(VTK_MODULE_ENABLE_VTK_RenderingCore YES CACHE STRING "Enable VTK RenderingCore" FORCE)
set(VTK_MODULE_ENABLE_VTK_RenderingOpenGL2 YES CACHE STRING "Enable VTK RenderingOpenGL2" FORCE)

add_subdirectory("${MEDCORE_VTK_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/third_party/vtk" EXCLUDE_FROM_ALL)

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
            "if (Test-Path '${CMAKE_CURRENT_BINARY_DIR}/third_party/vtk/bin/$<CONFIG>') { Get-ChildItem -Path '${CMAKE_CURRENT_BINARY_DIR}/third_party/vtk/bin/$<CONFIG>' -Filter '*.dll' | Copy-Item -Destination '$<TARGET_FILE_DIR:medCore>' -Force }"
        COMMENT "Deploying VTK runtime dependencies"
        VERBATIM
    )
endif()
