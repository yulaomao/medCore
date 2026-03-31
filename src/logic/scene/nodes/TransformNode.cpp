#include "TransformNode.h"
#include <QJsonArray>

TransformNode::TransformNode(QObject* parent)
    : NodeBase("TransformNode", parent)
{
    rebuildMatrixToParent();
}

QVector3D TransformNode::translation() const { return translation_; }
void TransformNode::setTranslation(const QVector3D& t) {
    translation_ = t;
    rebuildMatrixToParent();
    markDirty();
}

QVector3D TransformNode::rotation() const { return rotation_; }
void TransformNode::setRotation(const QVector3D& r) {
    rotation_ = r;
    rebuildMatrixToParent();
    markDirty();
}

QVector3D TransformNode::scale() const { return scale_; }
void TransformNode::setScale(const QVector3D& s) {
    scale_ = s;
    rebuildMatrixToParent();
    markDirty();
}

QMatrix4x4 TransformNode::toMatrix() const {
    return matrixToParent_;
}

QMatrix4x4 TransformNode::matrixToParent() const {
    return matrixToParent_;
}

void TransformNode::setMatrixToParent(const QMatrix4x4& matrix) {
    matrixToParent_ = matrix;
    translation_ = QVector3D(matrix(0, 3), matrix(1, 3), matrix(2, 3));
    markDirty();
}

QMatrix4x4 TransformNode::getMatrix() const {
    return matrixToParent();
}

void TransformNode::setMatrix(const QMatrix4x4& matrix) {
    setMatrixToParent(matrix);
}

QMatrix4x4 TransformNode::getInverseMatrix() const {
    return matrixToParent_.inverted();
}

QString TransformNode::parentTransformId() const {
    return parentTransformId_;
}

void TransformNode::setParentTransformId(const QString& parentId) {
    parentTransformId_ = parentId;
    markDirty();
}

QString TransformNode::getParentTransform() const {
    return parentTransformId();
}

void TransformNode::setParentTransform(const QString& parentId) {
    setParentTransformId(parentId);
}

QString TransformNode::transformKind() const {
    return transformKind_;
}

void TransformNode::setTransformKind(const QString& kind) {
    transformKind_ = kind;
    markDirty();
}

void TransformNode::rebuildMatrixToParent() {
    QMatrix4x4 m;
    m.translate(translation_);
    m.rotate(rotation_.z(), 0, 0, 1);
    m.rotate(rotation_.y(), 0, 1, 0);
    m.rotate(rotation_.x(), 1, 0, 0);
    m.scale(scale_);
    matrixToParent_ = m;
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
    obj["parentTransformId"] = parentTransformId_;
    obj["transformKind"] = transformKind_;
    QJsonArray matrixRows;
    for (int row = 0; row < 4; ++row) {
        QJsonArray matrixRow;
        for (int column = 0; column < 4; ++column)
            matrixRow.append(matrixToParent_(row, column));
        matrixRows.append(matrixRow);
    }
    obj["matrixToParent"] = matrixRows;
    return obj;
}

void TransformNode::fromJson(const QJsonObject& obj) {
    NodeBase::fromJson(obj);
    if (obj.contains("translation")) translation_ = decodeVec3(obj["translation"].toObject());
    if (obj.contains("rotation"))    rotation_    = decodeVec3(obj["rotation"].toObject());
    if (obj.contains("scale"))       scale_       = decodeVec3(obj["scale"].toObject());
    parentTransformId_ = obj["parentTransformId"].toString();
    transformKind_ = obj["transformKind"].toString(transformKind_);
    if (obj.contains("matrixToParent")) {
        const QJsonArray matrixRows = obj["matrixToParent"].toArray();
        QMatrix4x4 matrix;
        for (int row = 0; row < matrixRows.size() && row < 4; ++row) {
            const QJsonArray matrixRow = matrixRows.at(row).toArray();
            for (int column = 0; column < matrixRow.size() && column < 4; ++column)
                matrix(row, column) = static_cast<float>(matrixRow.at(column).toDouble());
        }
        matrixToParent_ = matrix;
    } else {
        rebuildMatrixToParent();
    }
}
