#include "PlanningModuleCoordinator.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>

PlanningModuleCoordinator::PlanningModuleCoordinator(VtkSceneWindow* sceneWindow,
                                                       QObject* parent)
    : ModuleCoordinator(parent)
    , sceneWindow_(sceneWindow)
{}

QString PlanningModuleCoordinator::moduleName() const { return "planning"; }

QWidget* PlanningModuleCoordinator::mainPage() {
    if (!page_) buildUi();
    return page_;
}

void PlanningModuleCoordinator::buildUi() {
    page_ = new QWidget();
    auto* mainLayout = new QVBoxLayout(page_);
    mainLayout->addWidget(new QLabel("<h2>Surgical Planning</h2>"));

    auto* splitter = new QSplitter(Qt::Horizontal);

    // Left panel: model + line lists
    auto* leftPanel  = new QWidget();
    auto* leftLayout = new QVBoxLayout(leftPanel);

    leftLayout->addWidget(new QLabel("Models:"));
    modelList_ = new QListWidget();
    leftLayout->addWidget(modelList_);
    auto* loadModelBtn = new QPushButton("Load Model...");
    leftLayout->addWidget(loadModelBtn);

    leftLayout->addWidget(new QLabel("Lines:"));
    lineList_ = new QListWidget();
    leftLayout->addWidget(lineList_);
    auto* addLineBtn = new QPushButton("Add Line");
    leftLayout->addWidget(addLineBtn);

    splitter->addWidget(leftPanel);
    if (sceneWindow_) splitter->addWidget(sceneWindow_);
    splitter->setStretchFactor(1, 3);

    mainLayout->addWidget(splitter);

    connect(loadModelBtn, &QPushButton::clicked, this, &PlanningModuleCoordinator::onLoadModelClicked);
    connect(addLineBtn,   &QPushButton::clicked, this, &PlanningModuleCoordinator::onAddLineClicked);
}

void PlanningModuleCoordinator::activate() {
    sendAction(ActionType::SyncRequest, QJsonObject());
}

void PlanningModuleCoordinator::onLoadModelClicked() {
    QString path = QFileDialog::getOpenFileName(page_, "Open Model", QString(),
                                                 "Mesh Files (*.stl *.obj *.vtk)");
    if (path.isEmpty()) return;
    QJsonObject payload;
    payload["command"]  = "loadModel";
    payload["filePath"] = path;
    sendAction(ActionType::UserInput, payload);
}

void PlanningModuleCoordinator::onAddLineClicked() {
    QJsonObject payload;
    payload["command"] = "addLine";
    QJsonObject start; start["x"] = 0; start["y"] = 0; start["z"] = 0;
    QJsonObject end;   end["x"]   = 10; end["y"]  = 10; end["z"]  = 10;
    payload["start"] = start;
    payload["end"]   = end;
    sendAction(ActionType::UserInput, payload);
}

void PlanningModuleCoordinator::onModuleNotification(const LogicNotification& notification) {
    if (notification.eventType != EventType::SceneUpdated) return;
    if (!notification.targetModules.contains(moduleName())) return;

    if (notification.payload.contains("modelId")) {
        QString fp = notification.payload["filePath"].toString();
        modelList_->addItem(fp.isEmpty() ? "Model" : fp);
    }
    if (notification.payload.contains("lineId")) {
        int count = notification.payload["count"].toInt();
        lineList_->clear();
        for (int i = 0; i < count; ++i)
            lineList_->addItem(QString("Line %1").arg(i + 1));
    }
}
