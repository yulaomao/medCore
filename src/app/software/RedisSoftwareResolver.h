// 文件说明：软件类型解析器声明，负责从 Redis 解析当前软件配置。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QObject>
#include <QString>

class RedisGateway;

/// 软件类型解析器。
///
/// 通过 RedisGateway 读取外部配置，决定当前应用应启用的初始化方案。
class RedisSoftwareResolver : public QObject {
    Q_OBJECT
public:
    // --- 解析入口 ---
    explicit RedisSoftwareResolver(QObject* parent = nullptr);
    ~RedisSoftwareResolver() override = default;

    QString resolve(const QString& serviceKey);

signals:
    void resolved(const QString& softwareType);
    void resolutionFailed(const QString& reason);

private:
    RedisGateway* gateway_;   // 用于读取外部软件类型配置的 Redis 网关。
};
