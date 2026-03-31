// 文件说明：工作区外壳声明，负责组织工具栏、中心页面与附属区域布局。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QWidget>
#include <QAction>
#include <QHBoxLayout>
#include <QFrame>
#include <QToolBar>
#include <QVBoxLayout>

/// 工作区外壳。
///
/// 负责组织模块导航工具栏、中心页面区域以及右侧/底部的附属面板。
class WorkspaceShell : public QWidget {
    Q_OBJECT
public:
    // --- 外壳生命周期与布局操作 ---
    explicit WorkspaceShell(QWidget* parent = nullptr);

    void addToolbarAction(QAction* action);
    void removeToolbarAction(QAction* action);
    void setPageArea(QWidget* pageWidget);
    void setRightArea(QWidget* widget);
    void setBottomArea(QWidget* widget);
    QWidget* rightArea() const;
    QWidget* bottomArea() const;

signals:
    void moduleNavigationRequested(const QString& moduleName);

private:
    QToolBar* toolbar_;              // 顶部模块导航工具栏。
    QVBoxLayout* layout_;            // 整体纵向布局。
    QHBoxLayout* centerLayout_;      // 中部水平布局，用于放置页面区与右侧区。
    QWidget* pageArea_{nullptr};     // 当前模块主页区域。
    QWidget* rightArea_{nullptr};    // 当前模块右侧附属区域。
    QWidget* bottomArea_{nullptr};   // 当前模块底部附属区域。
};
