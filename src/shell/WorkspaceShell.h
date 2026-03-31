#pragma once
#include <QWidget>
#include <QAction>
#include <QHBoxLayout>
#include <QFrame>
#include <QToolBar>
#include <QVBoxLayout>

class WorkspaceShell : public QWidget {
    Q_OBJECT
public:
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
    QToolBar* toolbar_;
    QVBoxLayout* layout_;
    QHBoxLayout* centerLayout_;
    QWidget* pageArea_{nullptr};
    QWidget* rightArea_{nullptr};
    QWidget* bottomArea_{nullptr};
};
