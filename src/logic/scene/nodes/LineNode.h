// 文件说明：线节点声明，负责折线数据管理和线条可视化属性控制。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include "NodeBase.h"
#include <QVector3D>
#include <QVector>
#include <QColor>

/// 线节点。
///
/// 用于保存折线或两点线段数据，并维护长度与渲染属性。
class LineNode : public NodeBase {
    Q_OBJECT
    Q_PROPERTY(bool closedFlag READ isClosed WRITE setClosed)
    Q_PROPERTY(QString lineRole READ lineRole WRITE setLineRole)
    Q_PROPERTY(double cachedLength READ getLength)
    Q_PROPERTY(QString parentTransformId READ parentTransformId WRITE setParentTransformId)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(double lineWidth READ lineWidth WRITE setLineWidth)
    Q_PROPERTY(bool visibilityFlag READ isVisible WRITE setVisibility)
    Q_PROPERTY(double opacityValue READ getOpacity WRITE setOpacity)
    Q_PROPERTY(QString renderMode READ renderMode WRITE setRenderMode)
    Q_PROPERTY(bool dashedFlag READ isDashed WRITE setDashed)

public:
    explicit LineNode(QObject* parent = nullptr);

    // --- 折线数据操作 ---
    void setPolyline(const QVector<QVector3D>& points);
    void appendVertex(const QVector3D& vertex);
    void removeVertex(int index);
    void clearVertices();
    int getVertexCount() const;
    QVector3D getVertex(int index) const;

    // --- 闭合状态 ---
    bool isClosed() const;
    void setClosed(bool closed);

    // --- 长度缓存 ---
    void recalculateLength();
    double getLength() const;

    // --- 线条角色 ---
    QString lineRole() const;
    void setLineRole(const QString& role);

    // --- 父变换关系 ---
    QString parentTransformId() const;
    void setParentTransformId(const QString& id);
    QString getParentTransform() const;
    void setParentTransform(const QString& id);

    // --- 可视化属性 ---
    QColor color() const;
    void setColor(const QColor& color);
    QColor getColor() const;
    double getOpacity() const;
    void setOpacity(double opacity);
    double lineWidth() const;
    void setLineWidth(double width);
    double getLineWidth() const;
    bool isVisible() const;
    void setVisibility(bool visible);
    QString renderMode() const;
    void setRenderMode(const QString& mode);
    QString getRenderMode() const;
    bool isDashed() const;
    void setDashed(bool dashed);

    // --- 便捷接口：两点线段 ---
    QVector3D startPoint() const;
    void setStartPoint(const QVector3D& pt);
    QVector3D endPoint() const;
    void setEndPoint(const QVector3D& pt);

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;

private:
    QVector<QVector3D> polylinePoints_;   // 折线顶点序列。
    bool closedFlag_{false};              // 是否闭合。
    QString lineRole_;                    // 线条业务角色。
    double cachedLength_{0.0};            // 当前缓存长度。
    QString parentTransformId_;           // 父变换节点标识。
    QColor color_{Qt::cyan};              // 线条颜色。
    double opacityValue_{1.0};            // 线条透明度。
    double lineWidthValue_{2.0};          // 线宽。
    QString renderMode_{"wireframe"};    // 渲染模式。
    bool dashedFlag_{false};              // 是否虚线显示。
    bool visibilityFlag_{true};           // 是否可见。
};
