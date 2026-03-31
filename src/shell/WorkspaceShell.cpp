// 文件说明：实现工作区外壳界面，组织工具栏、页面区域及右侧/底部附属区域。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "WorkspaceShell.h"

WorkspaceShell::WorkspaceShell(QWidget* parent)
    : QWidget(parent)
    , toolbar_(new QToolBar(this))
    , layout_(new QVBoxLayout(this))
    , centerLayout_(new QHBoxLayout())
{
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(0);
    layout_->addWidget(toolbar_);
    centerLayout_->setContentsMargins(0, 0, 0, 0);
    centerLayout_->setSpacing(0);
    layout_->addLayout(centerLayout_, 1);
    setLayout(layout_);
}

void WorkspaceShell::addToolbarAction(QAction* action) {
    toolbar_->addAction(action);
}

void WorkspaceShell::removeToolbarAction(QAction* action) {
    toolbar_->removeAction(action);
}

void WorkspaceShell::setPageArea(QWidget* pageWidget) {
    if (pageArea_) {
        centerLayout_->removeWidget(pageArea_);
        pageArea_->setParent(nullptr);
    }
    pageArea_ = pageWidget;
    if (pageArea_) {
        centerLayout_->insertWidget(0, pageArea_, 1);
    }
}

void WorkspaceShell::setRightArea(QWidget* widget) {
    if (rightArea_) {
        centerLayout_->removeWidget(rightArea_);
        rightArea_->setParent(nullptr);
    }
    rightArea_ = widget;
    if (rightArea_) {
        centerLayout_->addWidget(rightArea_);
    }
}

void WorkspaceShell::setBottomArea(QWidget* widget) {
    if (bottomArea_) {
        layout_->removeWidget(bottomArea_);
        bottomArea_->setParent(nullptr);
    }
    bottomArea_ = widget;
    if (bottomArea_) {
        layout_->addWidget(bottomArea_);
    }
}

QWidget* WorkspaceShell::rightArea() const {
    return rightArea_;
}

QWidget* WorkspaceShell::bottomArea() const {
    return bottomArea_;
}
