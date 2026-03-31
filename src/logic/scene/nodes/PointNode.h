#pragma once
#include "NodeBase.h"
#include <QVector3D>
#include <QVector>
#include <QColor>

struct PointItem {
    QString pointId;
    QString label;
    QVector3D position;
    bool selectedFlag{false};
    bool visibleFlag{true};
    bool lockedFlag{false};
    QString associatedNodeId;
    QColor colorRGBA{Qt::yellow};
    double sizeValue{3.0};
};

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

    // --- Multi-point container methods ---
    void addPoint(const PointItem& item);
    void insertPoint(int index, const PointItem& item);
    void removePoint(const QString& pointId);
    void removeAllPoints();
    int getPointCount() const;
    PointItem getPointByIndex(int index) const;
    PointItem getPointById(const QString& pointId) const;

    // --- Per-point property setters ---
    void setPointPosition(const QString& pointId, const QVector3D& pos);
    QVector3D getPointPosition(const QString& pointId) const;
    void setPointLabel(const QString& pointId, const QString& label);
    void setPointSelected(const QString& pointId, bool selected);
    void setPointLocked(const QString& pointId, bool locked);
    void setPointColor(const QString& pointId, const QColor& color);
    QColor getPointColor(const QString& pointId) const;
    void setPointSize(const QString& pointId, double size);
    double getPointSize(const QString& pointId) const;

    // --- Node-level properties ---
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

    // --- Visualization (per NodeBase convention) ---
    bool isVisible() const;
    void setVisibility(bool visible);
    double getOpacity() const;
    void setOpacity(double opacity);
    QColor color() const;
    void setColor(const QColor& color);
    QColor getColor() const;
    QString renderMode() const;
    void setRenderMode(const QString& mode);

    // --- Convenience: first-point accessors for simple single-point use ---
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

    QVector<PointItem> controlPoints_;
    QString pointLabelFormat_;
    int selectedPointIndex_{-1};
    QString pointRole_;
    QString parentTransformId_;
    QColor defaultPointColor_{Qt::yellow};
    double defaultPointSize_{3.0};
    bool showPointLabelFlag_{true};
    bool visibilityFlag_{true};
    double opacityValue_{1.0};
    QString renderMode_{"points"};
};
