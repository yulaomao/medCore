#pragma once
#include "NodeBase.h"
#include <QVector3D>
#include <QVector>
#include <QColor>

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

    // --- Polyline methods ---
    void setPolyline(const QVector<QVector3D>& points);
    void appendVertex(const QVector3D& vertex);
    void removeVertex(int index);
    void clearVertices();
    int getVertexCount() const;
    QVector3D getVertex(int index) const;

    // --- Closed/open ---
    bool isClosed() const;
    void setClosed(bool closed);

    // --- Length ---
    void recalculateLength();
    double getLength() const;

    // --- Line role ---
    QString lineRole() const;
    void setLineRole(const QString& role);

    // --- Parent transform ---
    QString parentTransformId() const;
    void setParentTransformId(const QString& id);
    QString getParentTransform() const;
    void setParentTransform(const QString& id);

    // --- Visualization ---
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

    // --- Convenience: simple two-point line ---
    QVector3D startPoint() const;
    void setStartPoint(const QVector3D& pt);
    QVector3D endPoint() const;
    void setEndPoint(const QVector3D& pt);

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;

private:
    QVector<QVector3D> polylinePoints_;
    bool closedFlag_{false};
    QString lineRole_;
    double cachedLength_{0.0};
    QString parentTransformId_;
    QColor color_{Qt::cyan};
    double opacityValue_{1.0};
    double lineWidthValue_{2.0};
    QString renderMode_{"wireframe"};
    bool dashedFlag_{false};
    bool visibilityFlag_{true};
};
