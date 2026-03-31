#pragma once
#include <QUuid>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QDateTime>
#include <QMetaType>

enum class EventType {
    Unknown,
    ModuleChanged,
    PageChanged,
    WorkflowChanged,
    ConnectionStateChanged,
    SceneNodesUpdated,
    StateChanged,
    ModuleReady,
    ModuleError,
    DataArrived,
    WorkflowAdvanced = WorkflowChanged,
    SceneUpdated = SceneNodesUpdated
};

enum class NotificationLevel {
    Info,
    Warning,
    Error,
    Critical
};

enum class TargetScope {
    Shell,
    CurrentModule,
    AllModules,
    ModuleList,
    All = AllModules,
    Module = ModuleList
};

struct LogicNotification {
    QUuid notificationId;
    QUuid sourceActionId;
    EventType eventType{EventType::Unknown};
    NotificationLevel level{NotificationLevel::Info};
    qint64 timestampMs{0};
    TargetScope targetScope{TargetScope::All};
    QStringList targetModules;
    QJsonObject payload;

    static LogicNotification create(EventType evType,
                                    NotificationLevel lvl,
                                    const QJsonObject& pl,
                                    TargetScope scope = TargetScope::All,
                                    const QStringList& modules = {},
                                    const QUuid& srcActionId = {})
    {
        LogicNotification n;
        n.notificationId = QUuid::createUuid();
        n.sourceActionId = srcActionId;
        n.eventType = evType;
        n.level = lvl;
        n.timestampMs = QDateTime::currentMSecsSinceEpoch();
        n.targetScope = scope;
        n.targetModules = modules;
        n.payload = pl;
        return n;
    }
};

Q_DECLARE_METATYPE(LogicNotification)
