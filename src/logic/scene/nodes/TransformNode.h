#pragma once
#include "NodeBase.h"
#include <QVector3D>
#include <QMatrix4x4>

class TransformNode : public NodeBase {
    Q_OBJECT
    Q_PROPERTY(QVector3D translation READ translation WRITE setTranslation)
    Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation)
    Q_PROPERTY(QVector3D scale READ scale WRITE setScale)
    Q_PROPERTY(QMatrix4x4 matrixToParent READ matrixToParent WRITE setMatrixToParent)
    Q_PROPERTY(QString parentTransformId READ parentTransformId WRITE setParentTransformId)
    Q_PROPERTY(QString transformKind READ transformKind WRITE setTransformKind)

public:
    explicit TransformNode(QObject* parent = nullptr);

    QVector3D translation() const;
    void setTranslation(const QVector3D& t);

    QVector3D rotation() const;  // euler angles in degrees
    void setRotation(const QVector3D& r);

    QVector3D scale() const;
    void setScale(const QVector3D& s);

    QMatrix4x4 toMatrix() const;
    QMatrix4x4 matrixToParent() const;
    void setMatrixToParent(const QMatrix4x4& matrix);
    QMatrix4x4 getMatrix() const;
    void setMatrix(const QMatrix4x4& matrix);
    QMatrix4x4 getInverseMatrix() const;
    QString parentTransformId() const;
    void setParentTransformId(const QString& parentId);
    QString getParentTransform() const;
    void setParentTransform(const QString& parentId);
    QString transformKind() const;
    void setTransformKind(const QString& kind);

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;

private:
    void rebuildMatrixToParent();

    QVector3D translation_;
    QVector3D rotation_;
    QVector3D scale_{1.0f, 1.0f, 1.0f};
    QMatrix4x4 matrixToParent_;
    QString parentTransformId_;
    QString transformKind_{"rigid"};
};
