// 文件说明：实现线节点的折线数据、显示参数和序列化逻辑。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "LineNode.h"
#include <QJsonArray>
#include <QtMath>

LineNode::LineNode(QObject* parent)
    : NodeBase("LineNode", parent)
{
    // 设计约定：LineNode 默认位于第 3 层
    defaultDisplayTarget_ = DisplayTarget{true, 3};
}

// --- 折线数据操作 ---

void LineNode::setPolyline(const QVector<QVector3D>& points) {
    polylinePoints_ = points;
    recalculateLength();
    markDirty();
}

void LineNode::appendVertex(const QVector3D& vertex) {
    polylinePoints_.append(vertex);
    recalculateLength();
    markDirty();
}

void LineNode::removeVertex(int index) {
    if (index >= 0 && index < polylinePoints_.size()) {
        polylinePoints_.removeAt(index);
        recalculateLength();
        markDirty();
    }
}

void LineNode::clearVertices() {
    if (polylinePoints_.isEmpty()) return;
    polylinePoints_.clear();
    cachedLength_ = 0.0;
    markDirty();
}

int LineNode::getVertexCount() const { return polylinePoints_.size(); }

QVector3D LineNode::getVertex(int index) const {
    if (index >= 0 && index < polylinePoints_.size())
        return polylinePoints_.at(index);
    return {};
}

// --- 闭合状态 ---

bool LineNode::isClosed() const { return closedFlag_; }
void LineNode::setClosed(bool closed) { closedFlag_ = closed; recalculateLength(); markDirty(); }

// --- 长度计算 ---

void LineNode::recalculateLength() {
    cachedLength_ = 0.0;
    for (int i = 1; i < polylinePoints_.size(); ++i) {
        cachedLength_ += polylinePoints_.at(i).distanceToPoint(polylinePoints_.at(i - 1));
    }
    if (closedFlag_ && polylinePoints_.size() > 1) {
        cachedLength_ += polylinePoints_.last().distanceToPoint(polylinePoints_.first());
    }
}

double LineNode::getLength() const { return cachedLength_; }

// --- 线条角色 ---

QString LineNode::lineRole() const { return lineRole_; }
void LineNode::setLineRole(const QString& role) { lineRole_ = role; markDirty(); }

// --- 父变换关系 ---

QString LineNode::parentTransformId() const { return parentTransformId_; }
void LineNode::setParentTransformId(const QString& id) { parentTransformId_ = id; markDirty(); }
QString LineNode::getParentTransform() const { return parentTransformId(); }
void LineNode::setParentTransform(const QString& id) { setParentTransformId(id); }

// --- 可视化属性 ---

QColor LineNode::color() const { return color_; }
void LineNode::setColor(const QColor& color) { color_ = color; markDirty(); }
QColor LineNode::getColor() const { return color(); }

double LineNode::getOpacity() const { return opacityValue_; }
void LineNode::setOpacity(double opacity) { opacityValue_ = qBound(0.0, opacity, 1.0); markDirty(); }

double LineNode::lineWidth() const { return lineWidthValue_; }
void LineNode::setLineWidth(double width) { lineWidthValue_ = width; markDirty(); }
double LineNode::getLineWidth() const { return lineWidth(); }

bool LineNode::isVisible() const { return visibilityFlag_; }
void LineNode::setVisibility(bool visible) { visibilityFlag_ = visible; markDirty(); }

QString LineNode::renderMode() const { return renderMode_; }
void LineNode::setRenderMode(const QString& mode) { renderMode_ = mode; markDirty(); }
QString LineNode::getRenderMode() const { return renderMode(); }

bool LineNode::isDashed() const { return dashedFlag_; }
void LineNode::setDashed(bool dashed) { dashedFlag_ = dashed; markDirty(); }

// --- 便捷接口：两点式访问器 ---

QVector3D LineNode::startPoint() const {
    return polylinePoints_.isEmpty() ? QVector3D() : polylinePoints_.first();
}

void LineNode::setStartPoint(const QVector3D& pt) {
    if (polylinePoints_.isEmpty()) {
        polylinePoints_.append(pt);
        polylinePoints_.append(QVector3D());
    } else {
        polylinePoints_[0] = pt;
    }
    recalculateLength();
    markDirty();
}

QVector3D LineNode::endPoint() const {
    return polylinePoints_.size() >= 2 ? polylinePoints_.last() : QVector3D();
}

void LineNode::setEndPoint(const QVector3D& pt) {
    if (polylinePoints_.isEmpty()) {
        polylinePoints_.append(QVector3D());
        polylinePoints_.append(pt);
    } else if (polylinePoints_.size() == 1) {
        polylinePoints_.append(pt);
    } else {
        polylinePoints_[polylinePoints_.size() - 1] = pt;
    }
    recalculateLength();
    markDirty();
}

// --- 序列化 ---

static QJsonObject encodeVec3L(const QVector3D& v) {
    QJsonObject o; o["x"] = v.x(); o["y"] = v.y(); o["z"] = v.z(); return o;
}
static QVector3D decodeVec3L(const QJsonObject& o) {
    return QVector3D(o["x"].toDouble(), o["y"].toDouble(), o["z"].toDouble());
}

QJsonObject LineNode::toJson() const {
    QJsonObject obj = NodeBase::toJson();
    QJsonArray pts;
    for (const auto& v : polylinePoints_) pts.append(encodeVec3L(v));
    obj["polylinePoints"] = pts;
    obj["closedFlag"] = closedFlag_;
    obj["lineRole"] = lineRole_;
    obj["cachedLength"] = cachedLength_;
    obj["parentTransformId"] = parentTransformId_;
    obj["color"] = color_.name();
    obj["opacityValue"] = opacityValue_;
    obj["lineWidthValue"] = lineWidthValue_;
    obj["renderMode"] = renderMode_;
    obj["dashedFlag"] = dashedFlag_;
    obj["visibilityFlag"] = visibilityFlag_;
    return obj;
}

void LineNode::fromJson(const QJsonObject& obj) {
    NodeBase::fromJson(obj);
    polylinePoints_.clear();
    if (obj.contains("polylinePoints")) {
        for (const auto& val : obj["polylinePoints"].toArray())
            polylinePoints_.append(decodeVec3L(val.toObject()));
    } else {
        // 向后兼容：旧版 startPoint/endPoint 格式
        if (obj.contains("startPoint"))
            polylinePoints_.append(decodeVec3L(obj["startPoint"].toObject()));
        if (obj.contains("endPoint"))
            polylinePoints_.append(decodeVec3L(obj["endPoint"].toObject()));
    }
    closedFlag_ = obj["closedFlag"].toBool(false);
    lineRole_ = obj["lineRole"].toString();
    parentTransformId_ = obj["parentTransformId"].toString();
    if (obj.contains("color")) color_ = QColor(obj["color"].toString());
    opacityValue_ = obj["opacityValue"].toDouble(1.0);
    lineWidthValue_ = obj["lineWidthValue"].toDouble(obj["lineWidth"].toDouble(2.0));
    renderMode_ = obj["renderMode"].toString("wireframe");
    dashedFlag_ = obj["dashedFlag"].toBool(false);
    visibilityFlag_ = obj["visibilityFlag"].toBool(true);
    recalculateLength();
}
