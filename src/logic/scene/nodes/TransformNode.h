// 文件说明：变换节点声明，负责维护位姿矩阵、空间信息与坐标轴显示。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include "NodeBase.h"
#include <QVector3D>
#include <QMatrix4x4>
#include <QColor>

/// 变换节点。
///
/// 同时维护平移、旋转、缩放与矩阵表示，用于表达坐标系之间的空间关系。
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

    QVector3D rotation() const;  // 欧拉角，单位为度。
    void setRotation(const QVector3D& r);

    QVector3D scale() const;
    void setScale(const QVector3D& s);

    // --- 矩阵访问 ---
    QMatrix4x4 toMatrix() const;
    QMatrix4x4 matrixToParent() const;
    void setMatrixToParent(const QMatrix4x4& matrix);
    void setMatrixTransformToParent(const QMatrix4x4& matrix);
    QMatrix4x4 getMatrixTransformToParent() const;
    QMatrix4x4 getMatrix() const;
    void setMatrix(const QMatrix4x4& matrix);
    QMatrix4x4 getInverseMatrix() const;

    // --- 变换运算 ---
    void multiplyBy(const QMatrix4x4& other);
    void inverse();
    bool isIdentity() const;
    QVector3D transformPoint(const QVector3D& point) const;
    QVector3D transformVector(const QVector3D& vector) const;

    // --- 父变换关系 ---
    QString parentTransformId() const;
    void setParentTransformId(const QString& parentId);
    QString getParentTransform() const;
    void setParentTransform(const QString& parentId);

    // --- 变换元数据 ---
    QString transformKind() const;
    void setTransformKind(const QString& kind);
    QString sourceSpaceName() const;
    void setSourceSpaceName(const QString& name);
    QString targetSpaceName() const;
    void setTargetSpaceName(const QString& name);

    // --- 坐标轴可视化 ---
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

    QVector3D translation_;                // 平移分量。
    QVector3D rotation_;                   // 旋转分量（欧拉角）。
    QVector3D scale_{1.0f, 1.0f, 1.0f};    // 缩放分量。
    QMatrix4x4 matrixToParent_;            // 到父坐标系的变换矩阵。
    QString parentTransformId_;            // 父变换节点标识。
    QString transformKind_{"rigid"};      // 变换类型，例如 rigid。
    QString sourceSpaceName_;              // 源空间名称。
    QString targetSpaceName_;              // 目标空间名称。
    bool showAxesFlag_{false};             // 是否显示坐标轴。
    double axesLengthValue_{50.0};         // 坐标轴长度。
    QColor axesColorX_{Qt::red};           // X 轴颜色。
    QColor axesColorY_{Qt::green};         // Y 轴颜色。
    QColor axesColorZ_{Qt::blue};          // Z 轴颜色。
};
