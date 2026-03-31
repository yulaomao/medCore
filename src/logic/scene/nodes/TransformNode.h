#pragma once
#include "NodeBase.h"
#include <QVector3D>
#include <QMatrix4x4>
#include <QColor>

class TransformNode : public NodeBase {
    Q_OBJECT
    Q_PROPERTY(QVector3D translation READ translation WRITE setTranslation)
    Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation)
    Q_PROPERTY(QVector3D scale READ scale WRITE setScale)
    Q_PROPERTY(QMatrix4x4 matrixToParent READ matrixToParent WRITE setMatrixToParent)
    Q_PROPERTY(QString parentTransformId READ parentTransformId WRITE setParentTransformId)
    Q_PROPERTY(QString transformKind READ transformKind WRITE setTransformKind)
    Q_PROPERTY(QString sourceSpaceName READ sourceSpaceName WRITE setSourceSpaceName)
    Q_PROPERTY(QString targetSpaceName READ targetSpaceName WRITE setTargetSpaceName)
    Q_PROPERTY(bool showAxesFlag READ isShowAxes WRITE setShowAxes)
    Q_PROPERTY(double axesLengthValue READ getAxesLength WRITE setAxesLength)

public:
    explicit TransformNode(QObject* parent = nullptr);

    QVector3D translation() const;
    void setTranslation(const QVector3D& t);

    QVector3D rotation() const;  // euler angles in degrees
    void setRotation(const QVector3D& r);

    QVector3D scale() const;
    void setScale(const QVector3D& s);

    // --- Matrix ---
    QMatrix4x4 toMatrix() const;
    QMatrix4x4 matrixToParent() const;
    void setMatrixToParent(const QMatrix4x4& matrix);
    void setMatrixTransformToParent(const QMatrix4x4& matrix);
    QMatrix4x4 getMatrixTransformToParent() const;
    QMatrix4x4 getMatrix() const;
    void setMatrix(const QMatrix4x4& matrix);
    QMatrix4x4 getInverseMatrix() const;

    // --- Transform operations ---
    void multiplyBy(const QMatrix4x4& other);
    void inverse();
    bool isIdentity() const;
    QVector3D transformPoint(const QVector3D& point) const;
    QVector3D transformVector(const QVector3D& vector) const;

    // --- Parent transform ---
    QString parentTransformId() const;
    void setParentTransformId(const QString& parentId);
    QString getParentTransform() const;
    void setParentTransform(const QString& parentId);

    // --- Transform metadata ---
    QString transformKind() const;
    void setTransformKind(const QString& kind);
    QString sourceSpaceName() const;
    void setSourceSpaceName(const QString& name);
    QString targetSpaceName() const;
    void setTargetSpaceName(const QString& name);

    // --- Axes visualization ---
    bool isShowAxes() const;
    void setShowAxes(bool show);
    double getAxesLength() const;
    void setAxesLength(double length);
    QColor getAxesColorX() const;
    void setAxesColorX(const QColor& color);
    QColor getAxesColorY() const;
    void setAxesColorY(const QColor& color);
    QColor getAxesColorZ() const;
    void setAxesColorZ(const QColor& color);

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
    QString sourceSpaceName_;
    QString targetSpaceName_;
    bool showAxesFlag_{false};
    double axesLengthValue_{50.0};
    QColor axesColorX_{Qt::red};
    QColor axesColorY_{Qt::green};
    QColor axesColorZ_{Qt::blue};
};
