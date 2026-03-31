#include "ModelNode.h"

ModelNode::ModelNode(QObject* parent)
    : NodeBase("ModelNode", parent)
{}

QString ModelNode::filePath() const { return filePath_; }
void ModelNode::setFilePath(const QString& path) { filePath_ = path; markDirty(); }

QColor ModelNode::color() const { return color_; }
void ModelNode::setColor(const QColor& color) { color_ = color; markDirty(); }

double ModelNode::opacity() const { return opacity_; }
void ModelNode::setOpacity(double opacity) { opacity_ = opacity; markDirty(); }

bool ModelNode::wireframe() const { return wireframe_; }
void ModelNode::setWireframe(bool wf) { wireframe_ = wf; markDirty(); }

QJsonObject ModelNode::toJson() const {
    QJsonObject obj = NodeBase::toJson();
    obj["filePath"]  = filePath_;
    obj["color"]     = color_.name();
    obj["opacity"]   = opacity_;
    obj["wireframe"] = wireframe_;
    return obj;
}

void ModelNode::fromJson(const QJsonObject& obj) {
    NodeBase::fromJson(obj);
    filePath_  = obj["filePath"].toString();
    if (obj.contains("color")) color_ = QColor(obj["color"].toString());
    opacity_   = obj["opacity"].toDouble(1.0);
    wireframe_ = obj["wireframe"].toBool(false);
}
