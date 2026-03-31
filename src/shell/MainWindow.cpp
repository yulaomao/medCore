// 文件说明：实现主窗口容器，承载工作区、全局覆盖层和浮动工具区域。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "MainWindow.h"
#include "WorkspaceShell.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QVBoxLayout>

MainWindow::MainWindow(WorkspaceShell* workspaceShell, QWidget* parent)
    : QMainWindow(parent)
    , workspaceShell_(workspaceShell)
    , rootStack_(new QStackedWidget(this))
    , globalOverlayLayer_(new QWidget(this))
    , globalToolHost_(new QWidget(this))
{
    setWindowTitle("medCore");
    resize(1280, 800);

    // 设计第 5 节要求：MainWindow 包含 rootStack、globalOverlayLayer 与 globalToolHost
    rootStack_->addWidget(workspaceShell_);
    setCentralWidget(rootStack_);

    setupOverlayLayers();
    setupMenuBar();
}

WorkspaceShell* MainWindow::workspaceShell() const {
    return workspaceShell_;
}

QWidget* MainWindow::globalOverlayLayer() const {
    return globalOverlayLayer_;
}

QWidget* MainWindow::globalToolHost() const {
    return globalToolHost_;
}

void MainWindow::setupMenuBar() {
    QMenu* fileMenu = menuBar()->addMenu("&File");

    QAction* quitAction = fileMenu->addAction("&Quit");
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, this, &MainWindow::shutdownRequested);
    connect(quitAction, &QAction::triggered, QApplication::instance(), &QApplication::quit);
}

void MainWindow::setupOverlayLayers() {
    // globalOverlayLayer 位于 rootStack 上层，用于承载通知、遮罩和确认对话框
    globalOverlayLayer_->setParent(this);
    globalOverlayLayer_->setAttribute(Qt::WA_TranslucentBackground, true);
    globalOverlayLayer_->hide();
    globalOverlayLayer_->raise();

    // globalToolHost 用于承载浮动工具窗口，例如三维工具面板
    globalToolHost_->setParent(this);
    globalToolHost_->setAttribute(Qt::WA_TranslucentBackground, true);
    globalToolHost_->hide();
    globalToolHost_->raise();
}
