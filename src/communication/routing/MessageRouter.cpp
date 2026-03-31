// 文件说明：实现消息路由器，按通道模式分发消息并校验消息包结构。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "MessageRouter.h"
#include <QMetaObject>
#include <QUuid>
#include <QDateTime>
#include <algorithm>

MessageRouter::MessageRouter(QObject* parent) : QObject(parent) {}

void MessageRouter::routeMessage(const QString& channel, const QJsonObject& message) {
    cleanupStaleRoutes();

    // 如果消息包含包络字段，则按设计 19.3 进行校验
    if (message.contains("msgId")) {
        QString validationError;
        if (!validateEnvelope(message, &validationError)) {
            emit envelopeValidationFailed(channel, validationError);
        }
    }

    bool routed = false;
    for (const auto& route : routes_) {
        if (route.receiver && route.pattern.match(channel).hasMatch()) {
            // 去掉槽函数签名前面的标记数字，SLOT() 宏会附加前缀 "1"
            QString slotName = route.slot;
            if (!slotName.isEmpty() && slotName[0].isDigit())
                slotName.remove(0, 1);
            QMetaObject::invokeMethod(route.receiver, slotName.toUtf8().constData(),
                                      Qt::QueuedConnection,
                                      Q_ARG(QString, channel),
                                      Q_ARG(QJsonObject, message));
            routed = true;
        }
    }
    if (!routed) emit unroutedMessage(channel, message);
}

void MessageRouter::registerRoute(const QString& channelPattern, QObject* receiver, const char* slot) {
    if (!receiver || !slot) return;

    Route r;
    // 将 glob 风格通道模式转换为正则表达式：* 对应 .*
    QString regexStr = QRegularExpression::escape(channelPattern);
    regexStr.replace("\\*", ".*");
    r.pattern  = QRegularExpression("^" + regexStr + "$");
    r.receiver = receiver;
    r.slot     = QString::fromUtf8(slot);
    routes_.append(r);

    connect(receiver, &QObject::destroyed, this, [this]() {
        hasStaleRoutes_ = true;
    });
}

void MessageRouter::unregisterRoutes(QObject* receiver) {
    routes_.erase(
        std::remove_if(routes_.begin(), routes_.end(),
                       [receiver](const Route& r) { return r.receiver == receiver; }),
        routes_.end());
}

bool MessageRouter::validateEnvelope(const QJsonObject& envelope, QString* errorOut) {
    // 设计 19.3 要求：msgId 必填且全局唯一，version 也必须存在
    if (!envelope.contains("msgId") || envelope["msgId"].toString().isEmpty()) {
        if (errorOut) *errorOut = "Missing or empty msgId";
        return false;
    }
    if (!envelope.contains("version") || envelope["version"].toString().isEmpty()) {
        if (errorOut) *errorOut = "Missing or empty version";
        return false;
    }
    if (!envelope.contains("msgType") || envelope["msgType"].toString().isEmpty()) {
        if (errorOut) *errorOut = "Missing or empty msgType";
        return false;
    }
    return true;
}

QJsonObject MessageRouter::wrapEnvelope(const QString& msgType,
                                         const QJsonObject& body,
                                         const QString& source,
                                         const QString& target,
                                         bool needAck) {
    QJsonObject envelope;
    envelope["msgId"] = QUuid::createUuid().toString();
    envelope["msgType"] = msgType;
    envelope["traceId"] = QUuid::createUuid().toString();
    envelope["source"] = source;
    envelope["target"] = target;
    envelope["timestampMs"] = QDateTime::currentMSecsSinceEpoch();
    envelope["version"] = "1.0";
    envelope["needAck"] = needAck;
    envelope["body"] = body;
    return envelope;
}

void MessageRouter::cleanupStaleRoutes() {
    if (!hasStaleRoutes_) return;

    routes_.erase(
        std::remove_if(routes_.begin(), routes_.end(),
                       [](const Route& route) { return route.receiver.isNull(); }),
        routes_.end());
    hasStaleRoutes_ = false;
}
