#pragma once
#include <QObject>
#include <QJsonObject>
#include <QPointer>
#include <QVector>
#include <QRegularExpression>

class MessageRouter : public QObject {
    Q_OBJECT
public:
    explicit MessageRouter(QObject* parent = nullptr);

    void routeMessage(const QString& channel, const QJsonObject& message);
    void registerRoute(const QString& channelPattern, QObject* receiver, const char* slot);
    void unregisterRoutes(QObject* receiver);

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
        QRegularExpression pattern;
        QPointer<QObject> receiver;
        QString slot;
    };
    QVector<Route> routes_;
    bool hasStaleRoutes_{false};
};
