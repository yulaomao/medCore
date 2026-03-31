// 文件说明：消息路由器声明，负责按通道模式分发 JSON 消息。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QObject>
#include <QJsonObject>
#include <QPointer>
#include <QVector>
#include <QRegularExpression>

/// 消息路由器。
///
/// 根据通道匹配规则将 JSON 消息分发到已注册的槽函数。
class MessageRouter : public QObject {
    Q_OBJECT
public:
    // --- 路由注册与消息分发 ---
    explicit MessageRouter(QObject* parent = nullptr);

    void routeMessage(const QString& channel, const QJsonObject& message);
    void registerRoute(const QString& channelPattern, QObject* receiver, const char* slot);
    void unregisterRoutes(QObject* receiver);

    // --- 包络校验与封装辅助 ---
    static bool validateEnvelope(const QJsonObject& envelope, QString* errorOut = nullptr);
    static QJsonObject wrapEnvelope(const QString& msgType,
                                     const QJsonObject& body,
                                     const QString& source = "desktop-client",
                                     const QString& target = "logic-runtime",
                                     bool needAck = false);

signals:
    void unroutedMessage(const QString& channel, const QJsonObject& message);
    void envelopeValidationFailed(const QString& channel, const QString& error);

private:
    void cleanupStaleRoutes();

    struct Route {
        QRegularExpression pattern;   // 通道匹配表达式。
        QPointer<QObject> receiver;   // 目标接收者；对象销毁后会自动失效。
        QString slot;                 // 目标槽函数签名。
    };
    QVector<Route> routes_;          // 已注册路由集合。
    bool hasStaleRoutes_{false};     // 是否存在待清理的失效路由。
};
