#pragma once
#include "../../ui/coordination/ModuleCoordinator.h"
#include "../../ui/vtk3d/VtkSceneWindow.h"
#include <QLabel>

class NavigationModuleCoordinator : public ModuleCoordinator {
    Q_OBJECT
public:
    explicit NavigationModuleCoordinator(VtkSceneWindow* sceneWindow = nullptr,
                                          QObject* parent = nullptr);

    QString moduleName() const override;
    QWidget* mainPage() override;
    void onModuleNotification(const LogicNotification& notification) override;
    void activate() override;

private:
    void buildUi();
    void updatePoseDisplay(const QJsonObject& payload);

    QWidget* page_{nullptr};
    QLabel* positionLabel_{nullptr};
    QLabel* rotationLabel_{nullptr};
    VtkSceneWindow* sceneWindow_;
};
