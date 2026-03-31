// 文件说明：实现基于 Redis 发布订阅的通信网关与 RESP 消息解析逻辑。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

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
    // 按 RESP 协议拼装 PUBLISH channel message 命令
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
    // 重新订阅当前已跟踪的通道
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
    // 以最小实现解析订阅模式下 Redis 推送的 RESP 消息。
    // 格式：*3\r\n$7\r\nmessage\r\n$<len>\r\n<channel>\r\n$<len>\r\n<data>\r\n
    while (socket_->bytesAvailable() > 0) {
        // 持续读取，直到拿到完整的 *3 数组头
        if (!socket_->canReadLine()) break;
        QString header = QString::fromUtf8(socket_->readLine()).trimmed();
        if (!header.startsWith('*')) continue;

        int parts = header.mid(1).toInt();
        QStringList tokens;
        bool ok = true;
        for (int i = 0; i < parts && ok; ++i) {
            if (!socket_->canReadLine()) { ok = false; break; }
            socket_->readLine(); // 跳过 $<len> 这一行
            if (!socket_->canReadLine()) { ok = false; break; }
            tokens << QString::fromUtf8(socket_->readLine()).trimmed();
        }
        if (!ok) break;

        // 期望得到的数组结构为 ["message", channel, data]
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
