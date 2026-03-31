#include "PointNode.h"
#include <QJsonObject>

PointNode::PointNode(QObject* parent)
    : NodeBase("PointNode", parent)
{}

QVector3D PointNode::position() const { return position_; }
void PointNode::setPosition(const QVector3D& pos) { position_ = pos; markDirty(); }

QString PointNode::label() const { return label_; }
void PointNode::setLabel(const QString& label) { label_ = label; markDirty(); }

QColor PointNode::color() const { return color_; }
void PointNode::setColor(const QColor& color) { color_ = color; markDirty(); }
QColor PointNode::getColor() const { return color(); }

double PointNode::radius() const { return radius_; }
void PointNode::setRadius(double radius) { radius_ = radius; markDirty(); }
bool PointNode::isVisible() const { return visibilityFlag_; }
void PointNode::setVisibility(bool visible) { visibilityFlag_ = visible; markDirty(); }
double PointNode::getOpacity() const { return opacityValue_; }
void PointNode::setOpacity(double opacity) { opacityValue_ = qBound(0.0, opacity, 1.0); markDirty(); }
QString PointNode::renderMode() const { return renderMode_; }
void PointNode::setRenderMode(const QString& mode) { renderMode_ = mode; markDirty(); }

QJsonObject PointNode::toJson() const {
    QJsonObject obj = NodeBase::toJson();
    QJsonObject pos;
    pos["x"] = position_.x(); pos["y"] = position_.y(); pos["z"] = position_.z();
    obj["position"] = pos;
    obj["label"] = label_;
    obj["color"] = color_.name();
    obj["radius"] = radius_;
    obj["visibilityFlag"] = visibilityFlag_;
    obj["opacityValue"] = opacityValue_;
    obj["renderMode"] = renderMode_;
    return obj;
}

void PointNode::fromJson(const QJsonObject& obj) {
    NodeBase::fromJson(obj);
    if (obj.contains("position")) {
        auto p = obj["position"].toObject();
        position_ = QVector3D(p["x"].toDouble(), p["y"].toDouble(), p["z"].toDouble());
    }
    label_ = obj["label"].toString();
    if (obj.contains("color")) color_ = QColor(obj["color"].toString());
    radius_ = obj["radius"].toDouble(3.0);
    visibilityFlag_ = obj["visibilityFlag"].toBool(true);
    opacityValue_ = obj["opacityValue"].toDouble(1.0);
    renderMode_ = obj["renderMode"].toString("points");
}
