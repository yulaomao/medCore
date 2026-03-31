#pragma once
#include <QUuid>
#include <QString>
#include <QJsonObject>
#include <QDateTime>
#include <QMetaType>

enum class ActionType {
    Unknown,
    NextStep,
    PreviousStep,
    RequestSwitchModule,
    ConfirmPoints,
    CancelSelection,
    StartNavigation,
    StopNavigation,
    LoadModel,
    RemoveModel,
    UserInput,
    SyncRequest,
    Shutdown,
    TriggerWorkflow,
    NavigateToModule = RequestSwitchModule
};

struct UiAction {
    QUuid actionId;
    ActionType actionType{ActionType::Unknown};
    QString module;
    qint64 timestampMs{0};
    QUuid traceId;
    QJsonObject payload;

    static UiAction create(ActionType type, const QString& mod, const QJsonObject& pl) {
        UiAction a;
        a.actionId = QUuid::createUuid();
        a.actionType = type;
        a.module = mod;
        a.timestampMs = QDateTime::currentMSecsSinceEpoch();
        a.traceId = QUuid::createUuid();
        a.payload = pl;
        return a;
    }
};

Q_DECLARE_METATYPE(UiAction)
