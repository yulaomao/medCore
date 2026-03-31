// 文件说明：点节点声明，负责多点容器管理和点级可视化属性控制。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include "NodeBase.h"
#include <QVector3D>
#include <QVector>
#include <QColor>

/// 单个控制点数据。
///
/// 保存点的标识、位置、显示状态以及与其他节点的关联信息。
struct PointItem {
    QString pointId;                  // 点唯一标识。
    QString label;                    // 点标签。
    QVector3D position;               // 点位置。
    bool selectedFlag{false};         // 是否被选中。
    bool visibleFlag{true};           // 是否可见。
    bool lockedFlag{false};           // 是否锁定。
    QString associatedNodeId;         // 关联节点标识。
    QColor colorRGBA{Qt::yellow};     // 点颜色。
    double sizeValue{3.0};            // 点尺寸。
};

/// 点节点。
///
/// 维护多点集合，并支持单点便捷访问和点级可视化配置。
class PointNode : public NodeBase {
    Q_OBJECT
    Q_PROPERTY(int pointCount READ getPointCount)
    Q_PROPERTY(QString pointRole READ pointRole WRITE setPointRole)
    Q_PROPERTY(QString parentTransformId READ parentTransformId WRITE setParentTransformId)
    Q_PROPERTY(QColor defaultPointColor READ defaultPointColor WRITE setDefaultPointColor)
    Q_PROPERTY(double defaultPointSize READ defaultPointSize WRITE setDefaultPointSize)
    Q_PROPERTY(bool showPointLabelFlag READ isShowPointLabel WRITE setShowPointLabel)
    Q_PROPERTY(bool visibilityFlag READ isVisible WRITE setVisibility)
    Q_PROPERTY(double opacityValue READ getOpacity WRITE setOpacity)
    Q_PROPERTY(QString renderMode READ renderMode WRITE setRenderMode)

public:
    explicit PointNode(QObject* parent = nullptr);

    // --- 多点容器操作 ---
    void addPoint(const PointItem& item);
    void insertPoint(int index, const PointItem& item);
    void removePoint(const QString& pointId);
    void removeAllPoints();
    int getPointCount() const;
    PointItem getPointByIndex(int index) const;
    PointItem getPointById(const QString& pointId) const;

    // --- 单点属性修改 ---
    void setPointPosition(const QString& pointId, const QVector3D& pos);
    QVector3D getPointPosition(const QString& pointId) const;
    void setPointLabel(const QString& pointId, const QString& label);
    void setPointSelected(const QString& pointId, bool selected);
    void setPointLocked(const QString& pointId, bool locked);
    void setPointColor(const QString& pointId, const QColor& color);
    QColor getPointColor(const QString& pointId) const;
    void setPointSize(const QString& pointId, double size);
    double getPointSize(const QString& pointId) const;

    // --- 节点级属性 ---
    QString pointLabelFormat() const;
    void setPointLabelFormat(const QString& format);
    int selectedPointIndex() const;
    void setSelectedPointIndex(int index);
    QString pointRole() const;
    void setPointRole(const QString& role);

    QString parentTransformId() const;
    void setParentTransformId(const QString& id);
    QString getParentTransform() const;
    void setParentTransform(const QString& id);

    QColor defaultPointColor() const;
    void setDefaultPointColor(const QColor& color);
    double defaultPointSize() const;
    void setDefaultPointSize(double size);
    bool isShowPointLabel() const;
    void setShowPointLabel(bool show);

    // --- 可视化属性 ---
    bool isVisible() const;
    void setVisibility(bool visible);
    double getOpacity() const;
    void setOpacity(double opacity);
    QColor color() const;
    void setColor(const QColor& color);
    QColor getColor() const;
    QString renderMode() const;
    void setRenderMode(const QString& mode);

    // --- 便捷接口：首个点访问 ---
    QVector3D position() const;
    void setPosition(const QVector3D& pos);
    QString label() const;
    void setLabel(const QString& label);
    double radius() const;
    void setRadius(double radius);

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;

private:
    int indexOfPointId(const QString& pointId) const;

    QVector<PointItem> controlPoints_;      // 点容器。
    QString pointLabelFormat_;              // 标签格式模板。
    int selectedPointIndex_{-1};            // 当前选中点的索引。
    QString pointRole_;                     // 点集合角色。
    QString parentTransformId_;             // 父变换节点标识。
    QColor defaultPointColor_{Qt::yellow};  // 默认点颜色。
    double defaultPointSize_{3.0};          // 默认点尺寸。
    bool showPointLabelFlag_{true};         // 是否显示点标签。
    bool visibilityFlag_{true};             // 节点整体可见性。
    double opacityValue_{1.0};              // 节点整体透明度。
    QString renderMode_{"points"};         // 点渲染模式。
};
