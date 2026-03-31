// 文件说明：导航模块界面协调器声明，负责展示导航状态与位姿信息。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include "../../ui/coordination/ModuleCoordinator.h"
#include "../../ui/vtk3d/VtkSceneWindow.h"
#include <QLabel>

/// 导航模块界面协调器。
///
/// 构建导航信息面板，并负责将通知中的位姿数据显示到界面。
class NavigationModuleCoordinator : public ModuleCoordinator {
    Q_OBJECT
public:
    // --- 模块基础接口 ---
    explicit NavigationModuleCoordinator(VtkSceneWindow* sceneWindow = nullptr,
                                          QObject* parent = nullptr);

    QString moduleName() const override;
    QWidget* mainPage() override;
    void onModuleNotification(const LogicNotification& notification) override;
    void activate() override;

private:
    // --- 界面内部装配 ---
    void buildUi();
    void updatePoseDisplay(const QJsonObject& payload);

    QWidget* page_{nullptr};              // 模块主页。
    QLabel* statusLabel_{nullptr};        // 导航状态文字。
    QLabel* positionLabel_{nullptr};      // 平移坐标显示。
    QLabel* rotationLabel_{nullptr};      // 旋转角度显示。
    VtkSceneWindow* sceneWindow_;         // 模块绑定的三维窗口。
};
