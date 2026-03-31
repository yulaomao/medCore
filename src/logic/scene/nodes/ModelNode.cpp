// 文件说明：实现模型节点的网格元数据、渲染属性和序列化逻辑。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "ModelNode.h"
#include <QJsonArray>
#include <limits>

namespace {
QString normalizedRenderMode(const QString& mode) {
    if (mode == "wireframe" || mode == "points") return mode;
    return "surface";
}
}

ModelNode::ModelNode(QObject* parent)
    : NodeBase("ModelNode", parent)
{
    // 设计约定：ModelNode 默认位于第 1 层，作为基础模型层
    defaultDisplayTarget_ = DisplayTarget{true, 1};
}

// --- PolyData / 网格数据 ---

void ModelNode::setPolyData(const QByteArray& data) {
    polyDataPayload_ = data;
    markDirty();
}

QByteArray ModelNode::getPolyData() const { return polyDataPayload_; }

void ModelNode::clearPolyData() {
    polyDataPayload_.clear();
    vertices_.clear();
    indices_.clear();
    cachedBounds_.clear();
    markDirty();
}

void ModelNode::setMeshData(const QVector<QVector3D>& vertices, const QVector<int>& indices) {
    vertices_ = vertices;
    indices_ = indices;
    updateCachedBounds();
    markDirty();
}

QVector<QVector3D> ModelNode::getVertices() const { return vertices_; }
QVector<int> ModelNode::getIndices() const { return indices_; }

QVector<double> ModelNode::getBoundingBox() const {
    if (cachedBounds_.isEmpty()) {
        const_cast<ModelNode*>(this)->updateCachedBounds();
    }
    return cachedBounds_;
}

// --- 文件路径 ---

QString ModelNode::filePath() const { return filePath_; }
void ModelNode::setFilePath(const QString& path) { filePath_ = path; markDirty(); }

// --- 可视化属性 ---

QColor ModelNode::color() const { return colorValue_; }
void ModelNode::setColor(const QColor& color) { colorValue_ = color; markDirty(); }
QColor ModelNode::getColor() const { return color(); }

double ModelNode::opacity() const { return opacity_; }
void ModelNode::setOpacity(double opacity) { opacity_ = qBound(0.0, opacity, 1.0); markDirty(); }
double ModelNode::getOpacity() const { return opacity(); }

bool ModelNode::isVisible() const { return visibilityFlag_; }
void ModelNode::setVisibility(bool visible) { visibilityFlag_ = visible; markDirty(); }

bool ModelNode::wireframe() const { return wireframe_; }
void ModelNode::setWireframe(bool wf) {
    wireframe_ = wf;
    renderMode_ = wireframe_ ? "wireframe" : "surface";
    markDirty();
}

QString ModelNode::renderMode() const { return renderMode_; }
void ModelNode::setRenderMode(const QString& mode) {
    renderMode_ = normalizedRenderMode(mode);
    wireframe_ = (renderMode_ == "wireframe");
    markDirty();
}
QString ModelNode::getRenderMode() const { return renderMode(); }

// --- 模型元数据 ---

QString ModelNode::modelRole() const { return modelRole_; }
void ModelNode::setModelRole(const QString& role) { modelRole_ = role; markDirty(); }

QString ModelNode::parentTransformId() const { return parentTransformId_; }
void ModelNode::setParentTransformId(const QString& id) { parentTransformId_ = id; markDirty(); }
QString ModelNode::getParentTransform() const { return parentTransformId(); }
void ModelNode::setParentTransform(const QString& id) { setParentTransformId(id); }

// --- 边缘显示 ---

bool ModelNode::isShowEdges() const { return showEdgesFlag_; }
void ModelNode::setShowEdges(bool show) { showEdgesFlag_ = show; markDirty(); }

QColor ModelNode::getEdgeColor() const { return edgeColorRGBA_; }
void ModelNode::setEdgeColor(const QColor& color) { edgeColorRGBA_ = color; markDirty(); }

double ModelNode::getEdgeWidth() const { return edgeWidthValue_; }
void ModelNode::setEdgeWidth(double width) { edgeWidthValue_ = width; markDirty(); }

// --- 背面剔除 ---

bool ModelNode::isBackfaceCulling() const { return backfaceCullingFlag_; }
void ModelNode::setBackfaceCulling(bool cull) { backfaceCullingFlag_ = cull; markDirty(); }

// --- 标量着色 ---

bool ModelNode::isUseScalarColor() const { return useScalarColorFlag_; }
void ModelNode::setUseScalarColor(bool use) { useScalarColorFlag_ = use; markDirty(); }

QString ModelNode::getScalarColorMap() const { return scalarColorMapName_; }
void ModelNode::setScalarColorMap(const QString& mapName) { scalarColorMapName_ = mapName; markDirty(); }

// --- 包围盒 ---

void ModelNode::updateCachedBounds() {
    if (vertices_.isEmpty()) {
        cachedBounds_.clear();
        return;
    }
    double xmin = std::numeric_limits<double>::max(), xmax = std::numeric_limits<double>::lowest();
    double ymin = xmin, ymax = xmax;
    double zmin = xmin, zmax = xmax;
    for (const auto& v : vertices_) {
        if (v.x() < xmin) xmin = v.x(); if (v.x() > xmax) xmax = v.x();
        if (v.y() < ymin) ymin = v.y(); if (v.y() > ymax) ymax = v.y();
        if (v.z() < zmin) zmin = v.z(); if (v.z() > zmax) zmax = v.z();
    }
    cachedBounds_ = {xmin, xmax, ymin, ymax, zmin, zmax};
}

// --- 序列化 ---

QJsonObject ModelNode::toJson() const {
    QJsonObject obj = NodeBase::toJson();
    obj["filePath"]  = filePath_;
    obj["color"]     = colorValue_.name();
    obj["opacity"]   = opacity_;
    obj["visibilityFlag"] = visibilityFlag_;
    obj["wireframe"] = wireframe_;
    obj["renderMode"] = renderMode_;
    obj["modelRole"] = modelRole_;
    obj["parentTransformId"] = parentTransformId_;
    obj["showEdgesFlag"] = showEdgesFlag_;
    obj["edgeColorRGBA"] = edgeColorRGBA_.name(QColor::HexArgb);
    obj["edgeWidthValue"] = edgeWidthValue_;
    obj["backfaceCullingFlag"] = backfaceCullingFlag_;
    obj["useScalarColorFlag"] = useScalarColorFlag_;
    obj["scalarColorMapName"] = scalarColorMapName_;
    return obj;
}

void ModelNode::fromJson(const QJsonObject& obj) {
    NodeBase::fromJson(obj);
    filePath_  = obj["filePath"].toString();
    if (obj.contains("color")) colorValue_ = QColor(obj["color"].toString());
    opacity_   = obj["opacity"].toDouble(1.0);
    visibilityFlag_ = obj["visibilityFlag"].toBool(true);
    wireframe_ = obj["wireframe"].toBool(false);
    renderMode_ = normalizedRenderMode(
        obj["renderMode"].toString(wireframe_ ? "wireframe" : "surface"));
    modelRole_ = obj["modelRole"].toString();
    parentTransformId_ = obj["parentTransformId"].toString();
    showEdgesFlag_ = obj["showEdgesFlag"].toBool(false);
    if (obj.contains("edgeColorRGBA")) edgeColorRGBA_ = QColor(obj["edgeColorRGBA"].toString());
    edgeWidthValue_ = obj["edgeWidthValue"].toDouble(1.0);
    backfaceCullingFlag_ = obj["backfaceCullingFlag"].toBool(false);
    useScalarColorFlag_ = obj["useScalarColorFlag"].toBool(false);
    scalarColorMapName_ = obj["scalarColorMapName"].toString();
}
