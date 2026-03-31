// 文件说明：实现变换节点的矩阵运算、坐标轴显示与序列化逻辑。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "TransformNode.h"
#include <QJsonArray>

TransformNode::TransformNode(QObject* parent)
    : NodeBase("TransformNode", parent)
{
    // 设计约定：坐标轴可视化默认位于第 3 层
    defaultDisplayTarget_ = DisplayTarget{true, 3};
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

void TransformNode::setMatrixTransformToParent(const QMatrix4x4& matrix) {
    setMatrixToParent(matrix);
}

QMatrix4x4 TransformNode::getMatrixTransformToParent() const {
    return matrixToParent();
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

// --- 变换运算 ---

void TransformNode::multiplyBy(const QMatrix4x4& other) {
    matrixToParent_ = matrixToParent_ * other;
    translation_ = QVector3D(matrixToParent_(0, 3), matrixToParent_(1, 3), matrixToParent_(2, 3));
    markDirty();
}

void TransformNode::inverse() {
    matrixToParent_ = matrixToParent_.inverted();
    translation_ = QVector3D(matrixToParent_(0, 3), matrixToParent_(1, 3), matrixToParent_(2, 3));
    markDirty();
}

bool TransformNode::isIdentity() const {
    return matrixToParent_.isIdentity();
}

QVector3D TransformNode::transformPoint(const QVector3D& point) const {
    return matrixToParent_.map(point);
}

QVector3D TransformNode::transformVector(const QVector3D& vector) const {
    return matrixToParent_.mapVector(vector);
}

// --- 父变换关系 ---

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

// --- 变换元数据 ---

QString TransformNode::transformKind() const {
    return transformKind_;
}

void TransformNode::setTransformKind(const QString& kind) {
    transformKind_ = kind;
    markDirty();
}

QString TransformNode::sourceSpaceName() const { return sourceSpaceName_; }
void TransformNode::setSourceSpaceName(const QString& name) { sourceSpaceName_ = name; markDirty(); }

QString TransformNode::targetSpaceName() const { return targetSpaceName_; }
void TransformNode::setTargetSpaceName(const QString& name) { targetSpaceName_ = name; markDirty(); }

// --- 坐标轴可视化 ---

bool TransformNode::isShowAxes() const { return showAxesFlag_; }
void TransformNode::setShowAxes(bool show) { showAxesFlag_ = show; markDirty(); }

double TransformNode::getAxesLength() const { return axesLengthValue_; }
void TransformNode::setAxesLength(double length) { axesLengthValue_ = length; markDirty(); }

QColor TransformNode::getAxesColorX() const { return axesColorX_; }
void TransformNode::setAxesColorX(const QColor& color) { axesColorX_ = color; markDirty(); }

QColor TransformNode::getAxesColorY() const { return axesColorY_; }
void TransformNode::setAxesColorY(const QColor& color) { axesColorY_ = color; markDirty(); }

QColor TransformNode::getAxesColorZ() const { return axesColorZ_; }
void TransformNode::setAxesColorZ(const QColor& color) { axesColorZ_ = color; markDirty(); }

// --- 内部辅助逻辑 ---

void TransformNode::rebuildMatrixToParent() {
    QMatrix4x4 m;
    m.translate(translation_);
    m.rotate(rotation_.z(), 0, 0, 1);
    m.rotate(rotation_.y(), 0, 1, 0);
    m.rotate(rotation_.x(), 1, 0, 0);
    m.scale(scale_);
    matrixToParent_ = m;
}

// --- 序列化 ---

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
    obj["sourceSpaceName"] = sourceSpaceName_;
    obj["targetSpaceName"] = targetSpaceName_;
    obj["showAxesFlag"] = showAxesFlag_;
    obj["axesLengthValue"] = axesLengthValue_;
    obj["axesColorX"] = axesColorX_.name();
    obj["axesColorY"] = axesColorY_.name();
    obj["axesColorZ"] = axesColorZ_.name();
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
    sourceSpaceName_ = obj["sourceSpaceName"].toString();
    targetSpaceName_ = obj["targetSpaceName"].toString();
    showAxesFlag_ = obj["showAxesFlag"].toBool(false);
    axesLengthValue_ = obj["axesLengthValue"].toDouble(50.0);
    if (obj.contains("axesColorX")) axesColorX_ = QColor(obj["axesColorX"].toString());
    if (obj.contains("axesColorY")) axesColorY_ = QColor(obj["axesColorY"].toString());
    if (obj.contains("axesColorZ")) axesColorZ_ = QColor(obj["axesColorZ"].toString());
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
