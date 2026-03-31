#pragma once
#include <QObject>
#include <QString>

class RedisGateway;

class RedisSoftwareResolver : public QObject {
    Q_OBJECT
public:
    explicit RedisSoftwareResolver(QObject* parent = nullptr);
    ~RedisSoftwareResolver() override = default;

    QString resolve(const QString& serviceKey);

signals:
    void resolved(const QString& softwareType);
    void resolutionFailed(const QString& reason);

private:
    RedisGateway* gateway_;
};
