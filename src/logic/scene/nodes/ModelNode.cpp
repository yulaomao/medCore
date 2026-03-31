#include "ModelNode.h"

namespace {
QString normalizedRenderMode(const QString& mode) {
    if (mode == "wireframe" || mode == "points") return mode;
    return "surface";
}
}

ModelNode::ModelNode(QObject* parent)
    : NodeBase("ModelNode", parent)
{}

QString ModelNode::filePath() const { return filePath_; }
void ModelNode::setFilePath(const QString& path) { filePath_ = path; markDirty(); }

QColor ModelNode::color() const { return color_; }
void ModelNode::setColor(const QColor& color) { color_ = color; markDirty(); }
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

QJsonObject ModelNode::toJson() const {
    QJsonObject obj = NodeBase::toJson();
    obj["filePath"]  = filePath_;
    obj["color"]     = color_.name();
    obj["opacity"]   = opacity_;
    obj["visibilityFlag"] = visibilityFlag_;
    obj["wireframe"] = wireframe_;
    obj["renderMode"] = renderMode_;
    return obj;
}

void ModelNode::fromJson(const QJsonObject& obj) {
    NodeBase::fromJson(obj);
    filePath_  = obj["filePath"].toString();
    if (obj.contains("color")) color_ = QColor(obj["color"].toString());
    opacity_   = obj["opacity"].toDouble(1.0);
    visibilityFlag_ = obj["visibilityFlag"].toBool(true);
    wireframe_ = obj["wireframe"].toBool(false);
    renderMode_ = normalizedRenderMode(
        obj["renderMode"].toString(wireframe_ ? "wireframe" : "surface"));
}
