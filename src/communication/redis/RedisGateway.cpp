#include "RedisGateway.h"
#include <QDebug>

RedisGateway::RedisGateway(QObject* parent)
    : QObject(parent)
    , socket_(new QTcpSocket(this))
{
    connect(socket_, &QTcpSocket::connected, this, &RedisGateway::onSocketConnected);
    connect(socket_, &QTcpSocket::disconnected, this, &RedisGateway::onSocketDisconnected);
    connect(socket_, &QTcpSocket::readyRead, this, &RedisGateway::onSocketReadyRead);
    connect(socket_, &QAbstractSocket::errorOccurred,
            this, &RedisGateway::onSocketError);
}

void RedisGateway::connectToRedis(const QString& host, quint16 port) {
    setState(State::Connecting);
    socket_->connectToHost(host, port);
}

void RedisGateway::disconnectFromRedis() {
    socket_->disconnectFromHost();
}

void RedisGateway::publish(const QString& channel, const QString& message) {
    if (state_ != State::Connected) return;
    // RESP protocol: PUBLISH channel message
    QString cmd = QString("*3\r\n$7\r\nPUBLISH\r\n$%1\r\n%2\r\n$%3\r\n%4\r\n")
        .arg(channel.size()).arg(channel).arg(message.size()).arg(message);
    socket_->write(cmd.toUtf8());
}

void RedisGateway::subscribe(const QString& channel) {
    if (!subscribedChannels_.contains(channel))
        subscribedChannels_.append(channel);
    if (state_ != State::Connected) return;
    QString cmd = QString("*2\r\n$9\r\nSUBSCRIBE\r\n$%1\r\n%2\r\n")
        .arg(channel.size()).arg(channel);
    socket_->write(cmd.toUtf8());
}

void RedisGateway::unsubscribe(const QString& channel) {
    subscribedChannels_.removeAll(channel);
    if (state_ != State::Connected) return;
    QString cmd = QString("*2\r\n$11\r\nUNSUBSCRIBE\r\n$%1\r\n%2\r\n")
        .arg(channel.size()).arg(channel);
    socket_->write(cmd.toUtf8());
}

QString RedisGateway::connectionState() const { return stateToString(state_); }
RedisGateway::State RedisGateway::state() const { return state_; }

void RedisGateway::onSocketConnected() {
    setState(State::Connected);
    // Re-subscribe to tracked channels
    for (const auto& ch : subscribedChannels_) {
        QString cmd = QString("*2\r\n$9\r\nSUBSCRIBE\r\n$%1\r\n%2\r\n").arg(ch.size()).arg(ch);
        socket_->write(cmd.toUtf8());
    }
}

void RedisGateway::onSocketDisconnected() {
    setState(State::Disconnected);
}

void RedisGateway::onSocketError(QAbstractSocket::SocketError /*error*/) {
    setState(State::Error);
    qWarning() << "RedisGateway socket error:" << socket_->errorString();
}

void RedisGateway::onSocketReadyRead() {
    // Minimal RESP parser for subscribe push messages.
    // Format: *3\r\n$7\r\nmessage\r\n$<len>\r\n<channel>\r\n$<len>\r\n<data>\r\n
    while (socket_->bytesAvailable() > 0) {
        // Read until we have a full *3 array header
        if (!socket_->canReadLine()) break;
        QString header = QString::fromUtf8(socket_->readLine()).trimmed();
        if (!header.startsWith('*')) continue;

        int parts = header.mid(1).toInt();
        QStringList tokens;
        bool ok = true;
        for (int i = 0; i < parts && ok; ++i) {
            if (!socket_->canReadLine()) { ok = false; break; }
            socket_->readLine(); // skip $<len> line
            if (!socket_->canReadLine()) { ok = false; break; }
            tokens << QString::fromUtf8(socket_->readLine()).trimmed();
        }
        if (!ok) break;

        // Expect: ["message", channel, data]
        if (tokens.size() == 3 && tokens[0] == "message") {
            emit messageReceived(tokens[1], tokens[2]);
        }
    }
}

void RedisGateway::setState(State s) {
    if (state_ == s) return;
    state_ = s;
    emit connectionStateChanged(stateToString(s));
}

QString RedisGateway::stateToString(State s) {
    switch (s) {
        case State::Disconnected: return "Disconnected";
        case State::Connecting:   return "Connecting";
        case State::Connected:    return "Connected";
        case State::Error:        return "Error";
    }
    return "Unknown";
}
