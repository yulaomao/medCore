#include "SceneGraph.h"

SceneGraph::SceneGraph(QObject* parent) : QObject(parent) {}

void SceneGraph::addNode(QSharedPointer<NodeBase> node) {
    if (!node) return;
    const QString id = node->nodeId().toString();
    {
        QWriteLocker locker(&lock_);
        nodes_[id] = node;
    }
    connect(node.data(), &NodeBase::modified,
            this, &SceneGraph::onNodeModified, Qt::QueuedConnection);
    emit nodeAdded(id);
}

void SceneGraph::removeNode(const QString& nodeId) {
    QSharedPointer<NodeBase> node;
    {
        QWriteLocker locker(&lock_);
        node = nodes_.take(nodeId);
    }
    if (node) {
        disconnect(node.data(), &NodeBase::modified, this, &SceneGraph::onNodeModified);
        emit nodeRemoved(nodeId);
    }
}

QSharedPointer<NodeBase> SceneGraph::getNode(const QString& nodeId) const {
    QReadLocker locker(&lock_);
    return nodes_.value(nodeId);
}

QVector<QSharedPointer<NodeBase>> SceneGraph::getNodesByType(const QString& type) const {
    QReadLocker locker(&lock_);
    QVector<QSharedPointer<NodeBase>> result;
    for (const auto& node : nodes_) {
        if (node->nodeType() == type)
            result.append(node);
    }
    return result;
}

QVector<QSharedPointer<NodeBase>> SceneGraph::getAllNodes() const {
    QReadLocker locker(&lock_);
    return QVector<QSharedPointer<NodeBase>>(nodes_.values().begin(), nodes_.values().end());
}

void SceneGraph::onNodeModified(const QString& nodeId) {
    emit nodeModified(nodeId);
}
