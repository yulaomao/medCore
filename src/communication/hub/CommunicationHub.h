// 文件说明：通信枢纽声明，负责衔接数据源、消息路由与逻辑运行时。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QObject>
#include <QHash>
#include <QSharedPointer>
#include "../datasource/SourceBase.h"
#include "../routing/MessageRouter.h"

// 前向声明 LogicRuntime，避免与运行时头文件形成循环依赖。
class LogicRuntime;

/// 通信枢纽。
///
/// 负责集中管理数据源、接收采样结果并转发到消息路由器和逻辑运行时。
class CommunicationHub : public QObject {
    Q_OBJECT
public:
    // --- 依赖注入与生命周期 ---
    explicit CommunicationHub(QObject* parent = nullptr);
    ~CommunicationHub() override = default;

    void addDataSource(QSharedPointer<SourceBase> source);
    void removeDataSource(const QString& channelName);

    void setMessageRouter(MessageRouter* router);
    void setLogicRuntime(LogicRuntime* runtime);

    void start();
    void stop();

signals:
    void stateSampleReceived(const StateSample& sample);
    void dataSourceError(const QString& channelName, const QString& errorMessage);

private slots:
    void onSampleReady(const StateSample& sample);
    void onSourceError(const QString& channelName, const QString& errorMessage);

private:
    QHash<QString, QSharedPointer<SourceBase>> sources_;   // 以通道名索引的数据源集合。
    MessageRouter* router_{nullptr};                       // 消息路由器。
    LogicRuntime* logicRuntime_{nullptr};                  // 逻辑运行时。
};
