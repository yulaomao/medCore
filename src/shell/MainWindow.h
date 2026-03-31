#pragma once
#include <QMainWindow>
#include <QStackedWidget>

class WorkspaceShell;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(WorkspaceShell* workspaceShell, QWidget* parent = nullptr);
    WorkspaceShell* workspaceShell() const;
    QWidget* globalOverlayLayer() const;
    QWidget* globalToolHost() const;

signals:
    void shutdownRequested();

private:
    void setupMenuBar();
    void setupOverlayLayers();

    WorkspaceShell* workspaceShell_;
    QStackedWidget* rootStack_;
    QWidget* globalOverlayLayer_;
    QWidget* globalToolHost_;
};
