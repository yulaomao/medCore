// 文件说明：数据源基类声明，约束统一的启动、停止与采样上报接口。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QObject>
#include "StateSample.h"

/// 数据源抽象基类。
///
/// 所有订阅源与轮询源都通过该接口暴露统一的生命周期和错误上报能力。
class SourceBase : public QObject {
    Q_OBJECT
public:
    // --- 基础信息与生命周期 ---
    explicit SourceBase(const QString& channelName, QObject* parent = nullptr);
    ~SourceBase() override = default;

    QString channelName() const;
    QString lastError() const;

    virtual void start() = 0;
    virtual void stop()  = 0;

signals:
    void sampleReady(const StateSample& sample);
    void errorOccurred(const QString& channelName, const QString& errorMessage);

protected:
    void reportError(const QString& errorMessage);

private:
    QString channelName_;   // 数据源所属通道名称。
    QString lastError_;     // 最近一次错误信息。
};
