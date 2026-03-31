// 文件说明：规划模块界面协调器声明，负责规划页面与三维窗口联动。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include "../../ui/coordination/ModuleCoordinator.h"
#include "../../ui/vtk3d/VtkSceneWindow.h"
#include <QListWidget>
#include <QLabel>

/// 规划模块界面协调器。
///
/// 构建规划模块页面，转发加载模型与新增规划线等操作，并显示当前统计信息。
class PlanningModuleCoordinator : public ModuleCoordinator {
    Q_OBJECT
public:
    // --- 模块基础接口 ---
    explicit PlanningModuleCoordinator(VtkSceneWindow* sceneWindow = nullptr,
                                        QObject* parent = nullptr);

    QString moduleName() const override;
    QWidget* mainPage() override;
    void onModuleNotification(const LogicNotification& notification) override;
    void activate() override;

private slots:
    void onLoadModelClicked();
    void onAddLineClicked();

private:
    // --- 界面内部装配 ---
    void buildUi();

    QWidget* page_{nullptr};            // 模块主页。
    QLabel* summaryLabel_{nullptr};     // 底部摘要信息标签。
    QListWidget* modelList_{nullptr};   // 模型列表。
    QListWidget* lineList_{nullptr};    // 规划线列表。
    VtkSceneWindow* sceneWindow_;       // 模块绑定的三维窗口。
};
