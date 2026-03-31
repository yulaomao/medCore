// 文件说明：主窗口声明，负责承载工作区外壳、全局覆盖层与工具宿主区域。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QMainWindow>
#include <QStackedWidget>

class WorkspaceShell;

/// 主窗口容器。
///
/// 负责组合 WorkspaceShell、全局覆盖层与工具宿主区域，向上提供统一的桌面级窗口骨架。
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    // --- 窗口生命周期与基础访问 ---
    explicit MainWindow(WorkspaceShell* workspaceShell, QWidget* parent = nullptr);
    WorkspaceShell* workspaceShell() const;
    QWidget* globalOverlayLayer() const;
    QWidget* globalToolHost() const;

signals:
    void shutdownRequested();

private:
    // --- 内部装配 ---
    void setupMenuBar();
    void setupOverlayLayers();

    WorkspaceShell* workspaceShell_;     // 主工作区外壳。
    QStackedWidget* rootStack_;          // 根堆叠容器，用于承载工作区与全局覆盖层。
    QWidget* globalOverlayLayer_;        // 全局覆盖层，用于通知、遮罩和确认弹层。
    QWidget* globalToolHost_;            // 浮动工具宿主区域。
};
