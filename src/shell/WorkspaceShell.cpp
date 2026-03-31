#include "WorkspaceShell.h"

WorkspaceShell::WorkspaceShell(QWidget* parent)
    : QWidget(parent)
    , toolbar_(new QToolBar(this))
    , layout_(new QVBoxLayout(this))
{
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(0);
    layout_->addWidget(toolbar_);
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
        layout_->removeWidget(pageArea_);
        pageArea_->setParent(nullptr);
    }
    pageArea_ = pageWidget;
    if (pageArea_) {
        layout_->addWidget(pageArea_, 1);
    }
}
