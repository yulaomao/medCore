#pragma once
#include "../../ui/coordination/ModuleCoordinator.h"
#include "../../ui/vtk3d/VtkSceneWindow.h"
#include <QListWidget>

class PointPickModuleCoordinator : public ModuleCoordinator {
    Q_OBJECT
public:
    explicit PointPickModuleCoordinator(VtkSceneWindow* sceneWindow = nullptr,
                                        QObject* parent = nullptr);

    QString moduleName() const override;
    QWidget* mainPage() override;
    void onModuleNotification(const LogicNotification& notification) override;
    void activate() override;

private slots:
    void onAddPointClicked();
    void onClearPointsClicked();

private:
    void buildUi();

    QWidget* page_{nullptr};
    QListWidget* pointList_{nullptr};
    VtkSceneWindow* sceneWindow_;
};
