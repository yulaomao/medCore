// 文件说明：点采集模块界面协调器声明，负责点列表展示与窗口联动。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include "../../ui/coordination/ModuleCoordinator.h"
#include "../../ui/vtk3d/VtkSceneWindow.h"
#include <QLabel>
#include <QListWidget>

/// 点采集模块界面协调器。
///
/// 构建点列表与摘要信息，并驱动点采集相关的界面动作。
class PointPickModuleCoordinator : public ModuleCoordinator {
    Q_OBJECT
public:
    // --- 模块基础接口 ---
    explicit PointPickModuleCoordinator(VtkSceneWindow* sceneWindow = nullptr,
                                        QObject* parent = nullptr);

    QString moduleName() const override;
    QWidget* mainPage() override;
    void onModuleNotification(const LogicNotification& notification) override;
    void activate() override;

private slots:
    void onAddPointClicked();
    void onClearPointsClicked();

private:
    // --- 界面内部装配 ---
    void buildUi();

    QWidget* page_{nullptr};           // 模块主页。
    QLabel* summaryLabel_{nullptr};    // 点数量摘要。
    QListWidget* pointList_{nullptr};  // 点列表。
    VtkSceneWindow* sceneWindow_;      // 模块绑定的三维窗口。
};
