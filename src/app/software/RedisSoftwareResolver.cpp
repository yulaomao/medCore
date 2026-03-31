// 文件说明：实现基于 Redis 的软件类型解析器。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "RedisSoftwareResolver.h"
#include "../../communication/redis/RedisGateway.h"

RedisSoftwareResolver::RedisSoftwareResolver(QObject* parent)
    : QObject(parent)
    , gateway_(new RedisGateway(this))
{}

QString RedisSoftwareResolver::resolve(const QString& serviceKey) {
    Q_UNUSED(serviceKey)
    if (gateway_->state() != RedisGateway::State::Connected) {
        emit resolutionFailed("Redis not connected, using default");
        return "default";
    }
    // 在真实实现中，这里应发送 GET 命令并解析返回结果
    emit resolved("default");
    return "default";
}
