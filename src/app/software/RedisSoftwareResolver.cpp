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
    // In a real implementation, send a GET command and parse the response
    emit resolved("default");
    return "default";
}
