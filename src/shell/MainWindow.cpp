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

    // Design section 5: MainWindow contains rootStack, globalOverlayLayer, globalToolHost
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
    // globalOverlayLayer sits on top of the rootStack for notifications, masks, confirm dialogs
    globalOverlayLayer_->setParent(this);
    globalOverlayLayer_->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    globalOverlayLayer_->setAttribute(Qt::WA_TranslucentBackground, true);
    globalOverlayLayer_->raise();

    // globalToolHost hosts floating tool windows (3D tools, etc.)
    globalToolHost_->setParent(this);
    globalToolHost_->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    globalToolHost_->setAttribute(Qt::WA_TranslucentBackground, true);
    globalToolHost_->raise();
}
