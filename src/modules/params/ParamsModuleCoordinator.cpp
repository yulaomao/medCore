// 文件说明：实现参数模块界面协调器，负责表单展示与用户输入转发。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "ParamsModuleCoordinator.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QJsonObject>

ParamsModuleCoordinator::ParamsModuleCoordinator(QObject* parent)
    : ModuleCoordinator(parent)
{}

QString ParamsModuleCoordinator::moduleName() const { return "params"; }

QWidget* ParamsModuleCoordinator::mainPage() {
    if (!page_) buildUi();
    return page_;
}

void ParamsModuleCoordinator::buildUi() {
    page_ = new QWidget();
    auto* layout = new QVBoxLayout(page_);
    layout->addWidget(new QLabel("<h2>Parameters</h2>"));

    auto* formWidget = new QWidget();
    formLayout_ = new QFormLayout(formWidget);
    layout->addWidget(formWidget);
    layout->addStretch();

    addParamField("patientId");
    addParamField("studyDescription");
    addParamField("operatorName");

    rightPanel_ = new QLabel("当前模块负责基础参数录入与同步结果展示。");
    rightPanel_->setMinimumWidth(240);
    setAttachmentWidget(AttachmentSlot::Right, rightPanel_);
}

void ParamsModuleCoordinator::addParamField(const QString& key, const QString& defaultValue) {
    auto* edit = new QLineEdit(defaultValue);
    edit->setPlaceholderText(key);
    formLayout_->addRow(key + ":", edit);
    fields_[key] = edit;

    connect(edit, &QLineEdit::editingFinished, this, [this, key, edit]() {
        onParamEdited(key, edit->text());
    });
}

void ParamsModuleCoordinator::onParamEdited(const QString& key, const QString& value) {
    QJsonObject payload;
    payload[key] = value;
    sendAction(ActionType::UserInput, payload);
}

void ParamsModuleCoordinator::activate() {
    sendAction(ActionType::SyncRequest, QJsonObject());
}

void ParamsModuleCoordinator::onModuleNotification(const LogicNotification& notification) {
    if (notification.eventType != EventType::StateChanged) return;

    QJsonObject params = notification.payload["params"].toObject();
    for (auto it = params.begin(); it != params.end(); ++it) {
        if (fields_.contains(it.key()))
            fields_[it.key()]->setText(it.value().toString());
    }
}
