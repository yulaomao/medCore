#pragma once
#include <QObject>
#include <QJsonObject>
#include <QVector>
#include <QRegularExpression>

class MessageRouter : public QObject {
    Q_OBJECT
public:
    explicit MessageRouter(QObject* parent = nullptr);

    void routeMessage(const QString& channel, const QJsonObject& message);
    void registerRoute(const QString& channelPattern, QObject* receiver, const char* slot);
    void unregisterRoutes(QObject* receiver);

signals:
    void unroutedMessage(const QString& channel, const QJsonObject& message);

private:
    struct Route {
        QRegularExpression pattern;
        QObject* receiver;
        QString slot;
    };
    QVector<Route> routes_;
};
