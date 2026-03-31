#include "TransformNode.h"

TransformNode::TransformNode(QObject* parent)
    : NodeBase("TransformNode", parent)
{}

QVector3D TransformNode::translation() const { return translation_; }
void TransformNode::setTranslation(const QVector3D& t) { translation_ = t; markDirty(); }

QVector3D TransformNode::rotation() const { return rotation_; }
void TransformNode::setRotation(const QVector3D& r) { rotation_ = r; markDirty(); }

QVector3D TransformNode::scale() const { return scale_; }
void TransformNode::setScale(const QVector3D& s) { scale_ = s; markDirty(); }

QMatrix4x4 TransformNode::toMatrix() const {
    QMatrix4x4 m;
    m.translate(translation_);
    m.rotate(rotation_.z(), 0, 0, 1);
    m.rotate(rotation_.y(), 0, 1, 0);
    m.rotate(rotation_.x(), 1, 0, 0);
    m.scale(scale_);
    return m;
}

static QJsonObject encodeVec3(const QVector3D& v) {
    QJsonObject o; o["x"] = v.x(); o["y"] = v.y(); o["z"] = v.z(); return o;
}
static QVector3D decodeVec3(const QJsonObject& o) {
    return QVector3D(o["x"].toDouble(), o["y"].toDouble(), o["z"].toDouble());
}

QJsonObject TransformNode::toJson() const {
    QJsonObject obj = NodeBase::toJson();
    obj["translation"] = encodeVec3(translation_);
    obj["rotation"]    = encodeVec3(rotation_);
    obj["scale"]       = encodeVec3(scale_);
    return obj;
}

void TransformNode::fromJson(const QJsonObject& obj) {
    NodeBase::fromJson(obj);
    if (obj.contains("translation")) translation_ = decodeVec3(obj["translation"].toObject());
    if (obj.contains("rotation"))    rotation_    = decodeVec3(obj["rotation"].toObject());
    if (obj.contains("scale"))       scale_       = decodeVec3(obj["scale"].toObject());
}
