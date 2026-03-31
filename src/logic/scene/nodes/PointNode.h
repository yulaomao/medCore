#pragma once
#include "NodeBase.h"
#include <QVector3D>
#include <QColor>

class PointNode : public NodeBase {
    Q_OBJECT
    Q_PROPERTY(QVector3D position READ position WRITE setPosition)
    Q_PROPERTY(QString label READ label WRITE setLabel)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(double radius READ radius WRITE setRadius)
    Q_PROPERTY(bool visibilityFlag READ isVisible WRITE setVisibility)
    Q_PROPERTY(double opacityValue READ getOpacity WRITE setOpacity)
    Q_PROPERTY(QString renderMode READ renderMode WRITE setRenderMode)

public:
    explicit PointNode(QObject* parent = nullptr);

    QVector3D position() const;
    void setPosition(const QVector3D& pos);

    QString label() const;
    void setLabel(const QString& label);

    QColor color() const;
    void setColor(const QColor& color);
    QColor getColor() const;

    double radius() const;
    void setRadius(double radius);
    bool isVisible() const;
    void setVisibility(bool visible);
    double getOpacity() const;
    void setOpacity(double opacity);
    QString renderMode() const;
    void setRenderMode(const QString& mode);

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;

private:
    QVector3D position_;
    QString label_;
    QColor color_{Qt::yellow};
    double radius_{3.0};
    bool visibilityFlag_{true};
    double opacityValue_{1.0};
    QString renderMode_{"points"};
};
