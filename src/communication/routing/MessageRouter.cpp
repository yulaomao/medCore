#include "MessageRouter.h"
#include <QMetaObject>
#include <QUuid>
#include <QDateTime>
#include <algorithm>

MessageRouter::MessageRouter(QObject* parent) : QObject(parent) {}

void MessageRouter::routeMessage(const QString& channel, const QJsonObject& message) {
    cleanupStaleRoutes();

    // Validate envelope if present (design section 19.3)
    if (message.contains("msgId")) {
        QString validationError;
        if (!validateEnvelope(message, &validationError)) {
            emit envelopeValidationFailed(channel, validationError);
        }
    }

    bool routed = false;
    for (const auto& route : routes_) {
        if (route.receiver && route.pattern.match(channel).hasMatch()) {
            // Strip leading slot specifier digits (SLOT() macro adds "1" prefix)
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
    // Convert glob-style pattern to regex: * -> .*
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
    // Design section 19.3: msgId required and globally unique, version required
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
