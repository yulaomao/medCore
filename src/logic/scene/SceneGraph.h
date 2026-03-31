#pragma once
#include <QObject>
#include <QReadWriteLock>
#include <QHash>
#include <QVector>
#include <QSharedPointer>
#include "nodes/NodeBase.h"

class SceneGraph : public QObject {
    Q_OBJECT
public:
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
    mutable QReadWriteLock lock_;
    QHash<QString, QSharedPointer<NodeBase>> nodes_;
};
