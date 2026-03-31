#pragma once
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QStringList>

class RedisGateway : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString connectionState READ connectionState NOTIFY connectionStateChanged)
public:
    enum class State { Disconnected, Connecting, Connected, Error };

    explicit RedisGateway(QObject* parent = nullptr);
    ~RedisGateway() override = default;

    void connectToRedis(const QString& host, quint16 port);
    void disconnectFromRedis();

    void publish(const QString& channel, const QString& message);
    void subscribe(const QString& channel);
    void unsubscribe(const QString& channel);

    QString connectionState() const;
    State state() const;

signals:
    void messageReceived(const QString& channel, const QString& message);
    void connectionStateChanged(const QString& state);

private slots:
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);
    void onSocketReadyRead();

private:
    void setState(State state);
    static QString stateToString(State s);

    QTcpSocket* socket_;
    State state_{State::Disconnected};
    QStringList subscribedChannels_;
};
