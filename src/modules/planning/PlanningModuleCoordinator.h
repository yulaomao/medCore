#pragma once
#include "../../ui/coordination/ModuleCoordinator.h"
#include "../../ui/vtk3d/VtkSceneWindow.h"
#include <QListWidget>
#include <QLabel>

class PlanningModuleCoordinator : public ModuleCoordinator {
    Q_OBJECT
public:
    explicit PlanningModuleCoordinator(VtkSceneWindow* sceneWindow = nullptr,
                                        QObject* parent = nullptr);

    QString moduleName() const override;
    QWidget* mainPage() override;
    void onModuleNotification(const LogicNotification& notification) override;
    void activate() override;

private slots:
    void onLoadModelClicked();
    void onAddLineClicked();

private:
    void buildUi();

    QWidget* page_{nullptr};
    QListWidget* modelList_{nullptr};
    QListWidget* lineList_{nullptr};
    VtkSceneWindow* sceneWindow_;
};
