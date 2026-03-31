// 文件说明：场景图声明，负责线程安全地集中管理所有场景节点。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QObject>
#include <QReadWriteLock>
#include <QHash>
#include <QVector>
#include <QSharedPointer>
#include "nodes/NodeBase.h"

/// 场景图容器。
///
/// 使用读写锁保护节点存储，为渲染层和逻辑层提供统一的节点查询入口。
class SceneGraph : public QObject {
    Q_OBJECT
public:
    // --- 节点增删查 ---
    explicit SceneGraph(QObject* parent = nullptr);

    void addNode(QSharedPointer<NodeBase> node);
    void removeNode(const QString& nodeId);
    QSharedPointer<NodeBase> getNode(const QString& nodeId) const;

    QVector<QSharedPointer<NodeBase>> getNodesByType(const QString& type) const;
    QVector<QSharedPointer<NodeBase>> getAllNodes() const;

signals:
    void nodeAdded(const QString& nodeId);
    void nodeRemoved(const QString& nodeId);
    void nodeModified(const QString& nodeId);

private slots:
    void onNodeModified(const QString& nodeId);

private:
    mutable QReadWriteLock lock_;                    // 保护节点容器的读写锁。
    QHash<QString, QSharedPointer<NodeBase>> nodes_; // 节点标识到节点实例的映射。
};
