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
    if (!notification.targetModules.contains(moduleName())) return;

    QJsonObject params = notification.payload["params"].toObject();
    for (auto it = params.begin(); it != params.end(); ++it) {
        if (fields_.contains(it.key()))
            fields_[it.key()]->setText(it.value().toString());
    }
}
