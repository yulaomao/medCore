#pragma once
#include <QMainWindow>

class WorkspaceShell;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(WorkspaceShell* workspaceShell, QWidget* parent = nullptr);
    WorkspaceShell* workspaceShell() const;

signals:
    void shutdownRequested();

private:
    void setupMenuBar();
    WorkspaceShell* workspaceShell_;
};
