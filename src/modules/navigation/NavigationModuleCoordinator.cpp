#include "NavigationModuleCoordinator.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QSplitter>

NavigationModuleCoordinator::NavigationModuleCoordinator(VtkSceneWindow* sceneWindow,
                                                           QObject* parent)
    : ModuleCoordinator(parent)
    , sceneWindow_(sceneWindow)
{}

QString NavigationModuleCoordinator::moduleName() const { return "navigation"; }

QWidget* NavigationModuleCoordinator::mainPage() {
    if (!page_) buildUi();
    return page_;
}

void NavigationModuleCoordinator::buildUi() {
    page_ = new QWidget();
    auto* mainLayout = new QVBoxLayout(page_);
    mainLayout->addWidget(new QLabel("<h2>Navigation</h2>"));

    auto* splitter = new QSplitter(Qt::Horizontal);

    // Info panel
    auto* infoPanel  = new QWidget();
    auto* infoLayout = new QVBoxLayout(infoPanel);

    auto* poseGroup  = new QGroupBox("Current Pose");
    auto* poseLayout = new QFormLayout(poseGroup);
    positionLabel_   = new QLabel("0.0, 0.0, 0.0");
    rotationLabel_   = new QLabel("0.0, 0.0, 0.0");
    poseLayout->addRow("Position (mm):", positionLabel_);
    poseLayout->addRow("Rotation (deg):", rotationLabel_);
    infoLayout->addWidget(poseGroup);
    infoLayout->addStretch();

    splitter->addWidget(infoPanel);
    if (sceneWindow_) splitter->addWidget(sceneWindow_);
    splitter->setStretchFactor(1, 3);

    mainLayout->addWidget(splitter);
}

void NavigationModuleCoordinator::activate() {
    sendAction(ActionType::SyncRequest, QJsonObject());
}

void NavigationModuleCoordinator::onModuleNotification(const LogicNotification& notification) {
    if (notification.eventType == EventType::SceneUpdated ||
        notification.eventType == EventType::DataArrived)
    {
        updatePoseDisplay(notification.payload);
    }
}

void NavigationModuleCoordinator::updatePoseDisplay(const QJsonObject& payload) {
    if (!positionLabel_ || !rotationLabel_) return;

    auto t = payload["translation"].toObject();
    auto r = payload["rotation"].toObject();

    positionLabel_->setText(QString("%1, %2, %3")
        .arg(t["x"].toDouble(), 0, 'f', 2)
        .arg(t["y"].toDouble(), 0, 'f', 2)
        .arg(t["z"].toDouble(), 0, 'f', 2));

    rotationLabel_->setText(QString("%1, %2, %3")
        .arg(r["x"].toDouble(), 0, 'f', 1)
        .arg(r["y"].toDouble(), 0, 'f', 1)
        .arg(r["z"].toDouble(), 0, 'f', 1));
}
