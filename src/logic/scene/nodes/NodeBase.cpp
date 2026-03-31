#include "NodeBase.h"

NodeBase::NodeBase(const QString& nodeType, QObject* parent)
    : QObject(parent)
    , nodeId_(QUuid::createUuid())
    , nodeType_(nodeType)
{}

QUuid NodeBase::nodeId() const { return nodeId_; }
QString NodeBase::nodeType() const { return nodeType_; }

DisplayTarget NodeBase::defaultDisplayTarget() const { return defaultDisplayTarget_; }

void NodeBase::setDefaultDisplayTarget(const DisplayTarget& target) {
    defaultDisplayTarget_ = target;
    markDirty();
}

DisplayTarget NodeBase::displayTargetForWindow(const QString& windowId) const {
    return windowDisplayOverrides_.value(windowId, defaultDisplayTarget_);
}

void NodeBase::setWindowDisplayOverride(const QString& windowId, const DisplayTarget& target) {
    windowDisplayOverrides_[windowId] = target;
    markDirty();
}

void NodeBase::removeWindowDisplayOverride(const QString& windowId) {
    windowDisplayOverrides_.remove(windowId);
    markDirty();
}

QHash<QString, DisplayTarget> NodeBase::windowDisplayOverrides() const {
    return windowDisplayOverrides_;
}

void NodeBase::beginModify() {
    ++modifyDepth_;
}

void NodeBase::endModify() {
    if (modifyDepth_ > 0) --modifyDepth_;
    if (modifyDepth_ == 0 && dirty_) {
        dirty_ = false;
        emit modified(nodeId_.toString());
    }
}

void NodeBase::markDirty() {
    if (modifyDepth_ > 0) {
        dirty_ = true;
    } else {
        emit modified(nodeId_.toString());
    }
}

QJsonObject NodeBase::toJson() const {
    QJsonObject obj;
    obj["nodeId"] = nodeId_.toString();
    obj["nodeType"] = nodeType_;
    QJsonObject dt;
    dt["visible"] = defaultDisplayTarget_.visible;
    dt["layer"] = defaultDisplayTarget_.layer;
    obj["defaultDisplayTarget"] = dt;
    return obj;
}

void NodeBase::fromJson(const QJsonObject& obj) {
    if (obj.contains("defaultDisplayTarget")) {
        auto dt = obj["defaultDisplayTarget"].toObject();
        defaultDisplayTarget_.visible = dt["visible"].toBool(true);
        defaultDisplayTarget_.layer = dt["layer"].toInt(0);
    }
}
