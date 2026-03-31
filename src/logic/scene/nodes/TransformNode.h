#pragma once
#include "NodeBase.h"
#include <QVector3D>
#include <QMatrix4x4>

class TransformNode : public NodeBase {
    Q_OBJECT
    Q_PROPERTY(QVector3D translation READ translation WRITE setTranslation)
    Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation)
    Q_PROPERTY(QVector3D scale READ scale WRITE setScale)

public:
    explicit TransformNode(QObject* parent = nullptr);

    QVector3D translation() const;
    void setTranslation(const QVector3D& t);

    QVector3D rotation() const;  // euler angles in degrees
    void setRotation(const QVector3D& r);

    QVector3D scale() const;
    void setScale(const QVector3D& s);

    QMatrix4x4 toMatrix() const;

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;

private:
    QVector3D translation_;
    QVector3D rotation_;
    QVector3D scale_{1.0f, 1.0f, 1.0f};
};
