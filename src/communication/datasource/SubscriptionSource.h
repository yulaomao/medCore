// 文件说明：订阅式数据源声明，利用定时器模拟持续到来的订阅样本。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include "SourceBase.h"
#include <QTimer>

/// 订阅式数据源。
///
/// 当前实现使用定时器模拟订阅通道中的持续采样推送。
class SubscriptionSource : public SourceBase {
    Q_OBJECT
public:
    // --- 生命周期 ---
    explicit SubscriptionSource(const QString& channelName,
                                 int intervalMs = 1000,
                                 QObject* parent = nullptr);
    ~SubscriptionSource() override = default;

    void start() override;
    void stop()  override;

private slots:
    void onTimer();

private:
    QTimer* timer_;               // 驱动模拟订阅采样的定时器。
    int intervalMs_;              // 推送间隔，单位毫秒。
    quint64 sequenceNumber_{0};   // 本地递增样本序号。
};
