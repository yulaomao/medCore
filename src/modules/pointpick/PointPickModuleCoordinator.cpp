// 文件说明：实现点采集模块界面协调器，负责点列表与三维窗口联动。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "PointPickModuleCoordinator.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QInputDialog>

PointPickModuleCoordinator::PointPickModuleCoordinator(VtkSceneWindow* sceneWindow,
                                                         QObject* parent)
    : ModuleCoordinator(parent)
    , sceneWindow_(sceneWindow)
{}

QString PointPickModuleCoordinator::moduleName() const { return "pointpick"; }

QWidget* PointPickModuleCoordinator::mainPage() {
    if (!page_) buildUi();
    return page_;
}

void PointPickModuleCoordinator::buildUi() {
    page_ = new QWidget();
    auto* layout = new QVBoxLayout(page_);
    layout->addWidget(new QLabel("<h2>Point Picking</h2>"));

    pointList_ = new QListWidget();
    layout->addWidget(pointList_);

    auto* btnLayout = new QHBoxLayout();
    auto* addBtn   = new QPushButton("Add Point");
    auto* clearBtn = new QPushButton("Clear All");
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(clearBtn);
    layout->addLayout(btnLayout);

    if (sceneWindow_) layout->addWidget(sceneWindow_);

    summaryLabel_ = new QLabel("点位数量：0");
    summaryLabel_->setMinimumWidth(220);
    setAttachmentWidget(AttachmentSlot::Right, summaryLabel_);

    connect(addBtn,   &QPushButton::clicked, this, &PointPickModuleCoordinator::onAddPointClicked);
    connect(clearBtn, &QPushButton::clicked, this, &PointPickModuleCoordinator::onClearPointsClicked);
}

void PointPickModuleCoordinator::activate() {
    sendAction(ActionType::SyncRequest, QJsonObject());
}

void PointPickModuleCoordinator::onAddPointClicked() {
    bool ok = false;
    QString label = QInputDialog::getText(page_, "Add Point", "Label:", QLineEdit::Normal, "P", &ok);
    if (!ok || label.isEmpty()) return;

    QJsonObject payload;
    payload["command"] = "addPoint";
    payload["label"]   = label;
    payload["x"]       = 0.0;
    payload["y"]       = 0.0;
    payload["z"]       = 0.0;
    sendAction(ActionType::UserInput, payload);
}

void PointPickModuleCoordinator::onClearPointsClicked() {
    QJsonObject payload;
    payload["command"] = "clearPoints";
    sendAction(ActionType::UserInput, payload);
}

void PointPickModuleCoordinator::onModuleNotification(const LogicNotification& notification) {
    if (notification.eventType != EventType::SceneUpdated) return;

    int count = notification.payload["count"].toInt();
    pointList_->clear();
    for (int i = 0; i < count; ++i)
        pointList_->addItem(QString("Point %1").arg(i + 1));
    if (summaryLabel_)
        summaryLabel_->setText(QString("点位数量：%1").arg(count));
}
