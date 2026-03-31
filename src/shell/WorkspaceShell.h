#pragma once
#include <QWidget>
#include <QAction>
#include <QToolBar>
#include <QVBoxLayout>

class WorkspaceShell : public QWidget {
    Q_OBJECT
public:
    explicit WorkspaceShell(QWidget* parent = nullptr);

    void addToolbarAction(QAction* action);
    void removeToolbarAction(QAction* action);
    void setPageArea(QWidget* pageWidget);

signals:
    void moduleNavigationRequested(const QString& moduleName);

private:
    QToolBar* toolbar_;
    QVBoxLayout* layout_;
    QWidget* pageArea_{nullptr};
};
