// 文件说明：Redis 网关声明，负责 TCP 连接状态与发布订阅操作。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QStringList>

/// Redis 通信网关。
///
/// 封装底层 TCP Socket 与 RESP 协议，用于完成发布订阅通信。
class RedisGateway : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString connectionState READ connectionState NOTIFY connectionStateChanged)
public:
    /// 与 Redis 的连接状态。
    enum class State { Disconnected, Connecting, Connected, Error };

    explicit RedisGateway(QObject* parent = nullptr);
    ~RedisGateway() override = default;

    // --- 连接与通道操作 ---
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

    QTcpSocket* socket_;                  // 底层 TCP 套接字。
    State state_{State::Disconnected};    // 当前连接状态。
    QStringList subscribedChannels_;      // 已订阅的通道列表，用于断线后恢复。
};
