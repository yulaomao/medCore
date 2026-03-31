#include "PointNode.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>

PointNode::PointNode(QObject* parent)
    : NodeBase("PointNode", parent)
{
    // Design: PointNode default layer = 3 to avoid occlusion by base models
    defaultDisplayTarget_ = DisplayTarget{true, 3};
}

// --- Multi-point container ---

void PointNode::addPoint(const PointItem& item) {
    PointItem pt = item;
    if (pt.pointId.isEmpty())
        pt.pointId = QUuid::createUuid().toString();
    controlPoints_.append(pt);
    markDirty();
}

void PointNode::insertPoint(int index, const PointItem& item) {
    PointItem pt = item;
    if (pt.pointId.isEmpty())
        pt.pointId = QUuid::createUuid().toString();
    if (index < 0 || index > controlPoints_.size())
        index = controlPoints_.size();
    controlPoints_.insert(index, pt);
    markDirty();
}

void PointNode::removePoint(const QString& pointId) {
    const int idx = indexOfPointId(pointId);
    if (idx >= 0) {
        controlPoints_.removeAt(idx);
        markDirty();
    }
}

void PointNode::removeAllPoints() {
    if (controlPoints_.isEmpty()) return;
    controlPoints_.clear();
    selectedPointIndex_ = -1;
    markDirty();
}

int PointNode::getPointCount() const {
    return controlPoints_.size();
}

PointItem PointNode::getPointByIndex(int index) const {
    if (index >= 0 && index < controlPoints_.size())
        return controlPoints_.at(index);
    return {};
}

PointItem PointNode::getPointById(const QString& pointId) const {
    const int idx = indexOfPointId(pointId);
    if (idx >= 0) return controlPoints_.at(idx);
    return {};
}

// --- Per-point property setters ---

void PointNode::setPointPosition(const QString& pointId, const QVector3D& pos) {
    const int idx = indexOfPointId(pointId);
    if (idx >= 0) { controlPoints_[idx].position = pos; markDirty(); }
}

QVector3D PointNode::getPointPosition(const QString& pointId) const {
    const int idx = indexOfPointId(pointId);
    return idx >= 0 ? controlPoints_.at(idx).position : QVector3D();
}

void PointNode::setPointLabel(const QString& pointId, const QString& label) {
    const int idx = indexOfPointId(pointId);
    if (idx >= 0) { controlPoints_[idx].label = label; markDirty(); }
}

void PointNode::setPointSelected(const QString& pointId, bool selected) {
    const int idx = indexOfPointId(pointId);
    if (idx >= 0) { controlPoints_[idx].selectedFlag = selected; markDirty(); }
}

void PointNode::setPointLocked(const QString& pointId, bool locked) {
    const int idx = indexOfPointId(pointId);
    if (idx >= 0) { controlPoints_[idx].lockedFlag = locked; markDirty(); }
}

void PointNode::setPointColor(const QString& pointId, const QColor& color) {
    const int idx = indexOfPointId(pointId);
    if (idx >= 0) { controlPoints_[idx].colorRGBA = color; markDirty(); }
}

QColor PointNode::getPointColor(const QString& pointId) const {
    const int idx = indexOfPointId(pointId);
    return idx >= 0 ? controlPoints_.at(idx).colorRGBA : defaultPointColor_;
}

void PointNode::setPointSize(const QString& pointId, double size) {
    const int idx = indexOfPointId(pointId);
    if (idx >= 0) { controlPoints_[idx].sizeValue = size; markDirty(); }
}

double PointNode::getPointSize(const QString& pointId) const {
    const int idx = indexOfPointId(pointId);
    return idx >= 0 ? controlPoints_.at(idx).sizeValue : defaultPointSize_;
}

// --- Node-level properties ---

QString PointNode::pointLabelFormat() const { return pointLabelFormat_; }
void PointNode::setPointLabelFormat(const QString& format) { pointLabelFormat_ = format; markDirty(); }

int PointNode::selectedPointIndex() const { return selectedPointIndex_; }
void PointNode::setSelectedPointIndex(int index) { selectedPointIndex_ = index; markDirty(); }

QString PointNode::pointRole() const { return pointRole_; }
void PointNode::setPointRole(const QString& role) { pointRole_ = role; markDirty(); }

QString PointNode::parentTransformId() const { return parentTransformId_; }
void PointNode::setParentTransformId(const QString& id) { parentTransformId_ = id; markDirty(); }
QString PointNode::getParentTransform() const { return parentTransformId(); }
void PointNode::setParentTransform(const QString& id) { setParentTransformId(id); }

QColor PointNode::defaultPointColor() const { return defaultPointColor_; }
void PointNode::setDefaultPointColor(const QColor& color) { defaultPointColor_ = color; markDirty(); }

double PointNode::defaultPointSize() const { return defaultPointSize_; }
void PointNode::setDefaultPointSize(double size) { defaultPointSize_ = size; markDirty(); }

bool PointNode::isShowPointLabel() const { return showPointLabelFlag_; }
void PointNode::setShowPointLabel(bool show) { showPointLabelFlag_ = show; markDirty(); }

// --- Visualization ---

bool PointNode::isVisible() const { return visibilityFlag_; }
void PointNode::setVisibility(bool visible) { visibilityFlag_ = visible; markDirty(); }
double PointNode::getOpacity() const { return opacityValue_; }
void PointNode::setOpacity(double opacity) { opacityValue_ = qBound(0.0, opacity, 1.0); markDirty(); }
QColor PointNode::color() const { return defaultPointColor_; }
void PointNode::setColor(const QColor& color) { setDefaultPointColor(color); }
QColor PointNode::getColor() const { return color(); }
QString PointNode::renderMode() const { return renderMode_; }
void PointNode::setRenderMode(const QString& mode) { renderMode_ = mode; markDirty(); }

// --- Convenience: first-point accessors ---

QVector3D PointNode::position() const {
    return controlPoints_.isEmpty() ? QVector3D() : controlPoints_.first().position;
}

void PointNode::setPosition(const QVector3D& pos) {
    if (controlPoints_.isEmpty()) {
        PointItem item;
        item.pointId = QUuid::createUuid().toString();
        item.position = pos;
        item.colorRGBA = defaultPointColor_;
        item.sizeValue = defaultPointSize_;
        controlPoints_.append(item);
    } else {
        controlPoints_[0].position = pos;
    }
    markDirty();
}

QString PointNode::label() const {
    return controlPoints_.isEmpty() ? QString() : controlPoints_.first().label;
}

void PointNode::setLabel(const QString& label) {
    if (controlPoints_.isEmpty()) {
        PointItem item;
        item.pointId = QUuid::createUuid().toString();
        item.label = label;
        item.colorRGBA = defaultPointColor_;
        item.sizeValue = defaultPointSize_;
        controlPoints_.append(item);
    } else {
        controlPoints_[0].label = label;
    }
    markDirty();
}

double PointNode::radius() const { return defaultPointSize_; }
void PointNode::setRadius(double radius) { setDefaultPointSize(radius); }

// --- Serialization ---

QJsonObject PointNode::toJson() const {
    QJsonObject obj = NodeBase::toJson();
    QJsonArray pts;
    for (const auto& pt : controlPoints_) {
        QJsonObject p;
        p["pointId"] = pt.pointId;
        p["label"] = pt.label;
        QJsonObject pos;
        pos["x"] = pt.position.x(); pos["y"] = pt.position.y(); pos["z"] = pt.position.z();
        p["position"] = pos;
        p["selectedFlag"] = pt.selectedFlag;
        p["visibleFlag"] = pt.visibleFlag;
        p["lockedFlag"] = pt.lockedFlag;
        p["associatedNodeId"] = pt.associatedNodeId;
        p["colorRGBA"] = pt.colorRGBA.name(QColor::HexArgb);
        p["sizeValue"] = pt.sizeValue;
        pts.append(p);
    }
    obj["controlPoints"] = pts;
    obj["pointLabelFormat"] = pointLabelFormat_;
    obj["selectedPointIndex"] = selectedPointIndex_;
    obj["pointRole"] = pointRole_;
    obj["parentTransformId"] = parentTransformId_;
    obj["defaultPointColor"] = defaultPointColor_.name(QColor::HexArgb);
    obj["defaultPointSize"] = defaultPointSize_;
    obj["showPointLabelFlag"] = showPointLabelFlag_;
    obj["visibilityFlag"] = visibilityFlag_;
    obj["opacityValue"] = opacityValue_;
    obj["renderMode"] = renderMode_;
    return obj;
}

void PointNode::fromJson(const QJsonObject& obj) {
    NodeBase::fromJson(obj);
    controlPoints_.clear();
    if (obj.contains("controlPoints")) {
        for (const auto& val : obj["controlPoints"].toArray()) {
            auto p = val.toObject();
            PointItem item;
            item.pointId = p["pointId"].toString();
            item.label = p["label"].toString();
            auto pos = p["position"].toObject();
            item.position = QVector3D(pos["x"].toDouble(), pos["y"].toDouble(), pos["z"].toDouble());
            item.selectedFlag = p["selectedFlag"].toBool(false);
            item.visibleFlag = p["visibleFlag"].toBool(true);
            item.lockedFlag = p["lockedFlag"].toBool(false);
            item.associatedNodeId = p["associatedNodeId"].toString();
            if (p.contains("colorRGBA")) item.colorRGBA = QColor(p["colorRGBA"].toString());
            item.sizeValue = p["sizeValue"].toDouble(3.0);
            controlPoints_.append(item);
        }
    } else if (obj.contains("position")) {
        // Backward compat: single-point format
        PointItem item;
        item.pointId = QUuid::createUuid().toString();
        auto pos = obj["position"].toObject();
        item.position = QVector3D(pos["x"].toDouble(), pos["y"].toDouble(), pos["z"].toDouble());
        item.label = obj["label"].toString();
        if (obj.contains("color")) item.colorRGBA = QColor(obj["color"].toString());
        item.sizeValue = obj["radius"].toDouble(3.0);
        item.visibleFlag = obj["visibilityFlag"].toBool(true);
        controlPoints_.append(item);
    }
    pointLabelFormat_ = obj["pointLabelFormat"].toString();
    selectedPointIndex_ = obj["selectedPointIndex"].toInt(-1);
    pointRole_ = obj["pointRole"].toString();
    parentTransformId_ = obj["parentTransformId"].toString();
    if (obj.contains("defaultPointColor"))
        defaultPointColor_ = QColor(obj["defaultPointColor"].toString());
    defaultPointSize_ = obj["defaultPointSize"].toDouble(3.0);
    showPointLabelFlag_ = obj["showPointLabelFlag"].toBool(true);
    visibilityFlag_ = obj["visibilityFlag"].toBool(true);
    opacityValue_ = obj["opacityValue"].toDouble(1.0);
    renderMode_ = obj["renderMode"].toString("points");
}

int PointNode::indexOfPointId(const QString& pointId) const {
    for (int i = 0; i < controlPoints_.size(); ++i) {
        if (controlPoints_.at(i).pointId == pointId)
            return i;
    }
    return -1;
}
