#include "MainWindow.h"
#include "WorkspaceShell.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QApplication>

MainWindow::MainWindow(WorkspaceShell* workspaceShell, QWidget* parent)
    : QMainWindow(parent)
    , workspaceShell_(workspaceShell)
{
    setWindowTitle("medCore");
    resize(1280, 800);
    setCentralWidget(workspaceShell_);
    setupMenuBar();
}

WorkspaceShell* MainWindow::workspaceShell() const {
    return workspaceShell_;
}

void MainWindow::setupMenuBar() {
    QMenu* fileMenu = menuBar()->addMenu("&File");

    QAction* quitAction = fileMenu->addAction("&Quit");
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, this, &MainWindow::shutdownRequested);
    connect(quitAction, &QAction::triggered, QApplication::instance(), &QApplication::quit);
}
