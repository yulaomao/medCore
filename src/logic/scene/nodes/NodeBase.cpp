#include "NodeBase.h"
#include <QJsonArray>

NodeBase::NodeBase(const QString& nodeType, QObject* parent)
    : QObject(parent)
    , nodeId_(QUuid::createUuid())
    , nodeType_(nodeType)
{}

QUuid NodeBase::nodeId() const { return nodeId_; }
QString NodeBase::nodeTagName() const { return nodeType_; }
QString NodeBase::nodeType() const { return nodeType_; }
QString NodeBase::name() const { return name_; }
void NodeBase::setName(const QString& name) { name_ = name; markDirty(); }
QString NodeBase::description() const { return description_; }
void NodeBase::setDescription(const QString& description) { description_ = description; markDirty(); }
int NodeBase::version() const { return version_; }
void NodeBase::setVersion(int version) { version_ = version; markDirty(); }

void NodeBase::setAttribute(const QString& key, const QJsonValue& value) {
    attributeMap_[key] = value;
    markDirty();
}

QJsonValue NodeBase::getAttribute(const QString& key, const QJsonValue& defaultValue) const {
    return attributeMap_.contains(key) ? attributeMap_.value(key) : defaultValue;
}

void NodeBase::removeAttribute(const QString& key) {
    if (!attributeMap_.contains(key)) return;
    attributeMap_.remove(key);
    markDirty();
}

QJsonObject NodeBase::attributeMap() const {
    return attributeMap_;
}

void NodeBase::addReference(const QString& relation, const QString& nodeId) {
    auto& refs = referenceMap_[relation];
    if (!refs.contains(nodeId)) {
        refs.append(nodeId);
        markDirty();
    }
}

void NodeBase::removeReference(const QString& relation, const QString& nodeId) {
    if (!referenceMap_.contains(relation)) return;
    referenceMap_[relation].removeAll(nodeId);
    if (referenceMap_[relation].isEmpty()) {
        referenceMap_.remove(relation);
    }
    markDirty();
}

QStringList NodeBase::getReferences(const QString& relation) const {
    return referenceMap_.value(relation);
}

QHash<QString, QStringList> NodeBase::referenceMap() const {
    return referenceMap_;
}

DisplayTarget NodeBase::defaultDisplayTarget() const { return defaultDisplayTarget_; }

void NodeBase::setDefaultDisplayTarget(const DisplayTarget& target) {
    defaultDisplayTarget_ = target;
    defaultDisplayTarget_.layer = qBound(1, defaultDisplayTarget_.layer, 3);
    markDirty();
}

DisplayTarget NodeBase::getDisplayTargetForWindow(const QString& windowId) const {
    return displayTargetForWindow(windowId);
}

DisplayTarget NodeBase::displayTargetForWindow(const QString& windowId) const {
    return windowDisplayOverrides_.value(windowId, defaultDisplayTarget_);
}

void NodeBase::setWindowDisplayTarget(const QString& windowId, const DisplayTarget& target) {
    setWindowDisplayOverride(windowId, target);
}

void NodeBase::setWindowDisplayOverride(const QString& windowId, const DisplayTarget& target) {
    DisplayTarget normalized = target;
    normalized.layer = qBound(1, normalized.layer, 3);
    windowDisplayOverrides_[windowId] = normalized;
    markDirty();
}

void NodeBase::removeWindowDisplayOverride(const QString& windowId) {
    windowDisplayOverrides_.remove(windowId);
    markDirty();
}

QHash<QString, DisplayTarget> NodeBase::windowDisplayOverrides() const {
    return windowDisplayOverrides_;
}

void NodeBase::startBatchModify() {
    ++modifyDepth_;
}

void NodeBase::beginModify() {
    startBatchModify();
}

void NodeBase::touchModified() {
    markDirty();
}

void NodeBase::endBatchModify() {
    if (modifyDepth_ > 0) --modifyDepth_;
    if (modifyDepth_ == 0 && dirty_) {
        dirty_ = false;
        emit modified(nodeId_.toString());
    }
}

void NodeBase::endModify() {
    endBatchModify();
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
    obj["nodeTagName"] = nodeType_;
    obj["nodeType"] = nodeType_;
    obj["name"] = name_;
    obj["description"] = description_;
    obj["version"] = version_;
    obj["attributeMap"] = attributeMap_;
    QJsonObject dt;
    dt["visible"] = defaultDisplayTarget_.visible;
    dt["layer"] = defaultDisplayTarget_.layer;
    obj["defaultDisplayTarget"] = dt;
    QJsonObject windows;
    for (auto it = windowDisplayOverrides_.cbegin(); it != windowDisplayOverrides_.cend(); ++it) {
        QJsonObject item;
        item["visible"] = it.value().visible;
        item["layer"] = it.value().layer;
        windows[it.key()] = item;
    }
    obj["windowDisplayOverrides"] = windows;
    QJsonObject refs;
    for (auto it = referenceMap_.cbegin(); it != referenceMap_.cend(); ++it) {
        QJsonArray arr;
        for (const auto& ref : it.value()) arr.append(ref);
        refs[it.key()] = arr;
    }
    obj["referenceMap"] = refs;
    return obj;
}

void NodeBase::fromJson(const QJsonObject& obj) {
    name_ = obj["name"].toString();
    description_ = obj["description"].toString();
    version_ = obj["version"].toInt(version_);
    attributeMap_ = obj["attributeMap"].toObject();
    if (obj.contains("defaultDisplayTarget")) {
        auto dt = obj["defaultDisplayTarget"].toObject();
        defaultDisplayTarget_.visible = dt["visible"].toBool(true);
        defaultDisplayTarget_.layer = qBound(1, dt["layer"].toInt(1), 3);
    }
    if (obj.contains("windowDisplayOverrides")) {
        windowDisplayOverrides_.clear();
        const auto windows = obj["windowDisplayOverrides"].toObject();
        for (auto it = windows.begin(); it != windows.end(); ++it) {
            const auto item = it.value().toObject();
            windowDisplayOverrides_[it.key()] = DisplayTarget{
                item["visible"].toBool(true),
                qBound(1, item["layer"].toInt(1), 3)
            };
        }
    }
    if (obj.contains("referenceMap")) {
        referenceMap_.clear();
        const auto refs = obj["referenceMap"].toObject();
        for (auto it = refs.begin(); it != refs.end(); ++it) {
            QStringList list;
            for (const auto& entry : it.value().toArray()) {
                list.append(entry.toString());
            }
            if (!list.isEmpty()) referenceMap_[it.key()] = list;
        }
    }
}
