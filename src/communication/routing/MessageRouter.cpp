#include "MessageRouter.h"
#include <QMetaObject>
#include <algorithm>

MessageRouter::MessageRouter(QObject* parent) : QObject(parent) {}

void MessageRouter::routeMessage(const QString& channel, const QJsonObject& message) {
    routes_.erase(
        std::remove_if(routes_.begin(), routes_.end(),
                       [](const Route& route) { return route.receiver.isNull(); }),
        routes_.end());

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

    connect(receiver, &QObject::destroyed, this, [this, receiver]() {
        unregisterRoutes(receiver);
    });
}

void MessageRouter::unregisterRoutes(QObject* receiver) {
    routes_.erase(
        std::remove_if(routes_.begin(), routes_.end(),
                       [receiver](const Route& r) { return r.receiver == receiver; }),
        routes_.end());
}
