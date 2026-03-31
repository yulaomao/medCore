#include "LineNode.h"

LineNode::LineNode(QObject* parent)
    : NodeBase("LineNode", parent)
{}

QVector3D LineNode::startPoint() const { return startPoint_; }
void LineNode::setStartPoint(const QVector3D& pt) { startPoint_ = pt; markDirty(); }

QVector3D LineNode::endPoint() const { return endPoint_; }
void LineNode::setEndPoint(const QVector3D& pt) { endPoint_ = pt; markDirty(); }

QColor LineNode::color() const { return color_; }
void LineNode::setColor(const QColor& color) { color_ = color; markDirty(); }
QColor LineNode::getColor() const { return color(); }

double LineNode::lineWidth() const { return lineWidth_; }
void LineNode::setLineWidth(double width) { lineWidth_ = width; markDirty(); }
bool LineNode::isVisible() const { return visibilityFlag_; }
void LineNode::setVisibility(bool visible) { visibilityFlag_ = visible; markDirty(); }
double LineNode::getOpacity() const { return opacityValue_; }
void LineNode::setOpacity(double opacity) { opacityValue_ = qBound(0.0, opacity, 1.0); markDirty(); }
QString LineNode::renderMode() const { return renderMode_; }
void LineNode::setRenderMode(const QString& mode) { renderMode_ = mode; markDirty(); }

QJsonObject LineNode::toJson() const {
    QJsonObject obj = NodeBase::toJson();
    auto encodeVec = [](const QVector3D& v) {
        QJsonObject o; o["x"] = v.x(); o["y"] = v.y(); o["z"] = v.z(); return o;
    };
    obj["startPoint"] = encodeVec(startPoint_);
    obj["endPoint"]   = encodeVec(endPoint_);
    obj["color"]      = color_.name();
    obj["lineWidth"]  = lineWidth_;
    obj["visibilityFlag"] = visibilityFlag_;
    obj["opacityValue"] = opacityValue_;
    obj["renderMode"] = renderMode_;
    return obj;
}

void LineNode::fromJson(const QJsonObject& obj) {
    NodeBase::fromJson(obj);
    auto decodeVec = [](const QJsonObject& o) {
        return QVector3D(o["x"].toDouble(), o["y"].toDouble(), o["z"].toDouble());
    };
    if (obj.contains("startPoint")) startPoint_ = decodeVec(obj["startPoint"].toObject());
    if (obj.contains("endPoint"))   endPoint_   = decodeVec(obj["endPoint"].toObject());
    if (obj.contains("color"))      color_      = QColor(obj["color"].toString());
    lineWidth_ = obj["lineWidth"].toDouble(2.0);
    visibilityFlag_ = obj["visibilityFlag"].toBool(true);
    opacityValue_ = obj["opacityValue"].toDouble(1.0);
    renderMode_ = obj["renderMode"].toString("wireframe");
}
