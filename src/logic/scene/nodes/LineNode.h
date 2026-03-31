#pragma once
#include "NodeBase.h"
#include <QVector3D>
#include <QColor>

class LineNode : public NodeBase {
    Q_OBJECT
    Q_PROPERTY(QVector3D startPoint READ startPoint WRITE setStartPoint)
    Q_PROPERTY(QVector3D endPoint READ endPoint WRITE setEndPoint)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(double lineWidth READ lineWidth WRITE setLineWidth)
    Q_PROPERTY(bool visibilityFlag READ isVisible WRITE setVisibility)
    Q_PROPERTY(double opacityValue READ getOpacity WRITE setOpacity)
    Q_PROPERTY(QString renderMode READ renderMode WRITE setRenderMode)

public:
    explicit LineNode(QObject* parent = nullptr);

    QVector3D startPoint() const;
    void setStartPoint(const QVector3D& pt);

    QVector3D endPoint() const;
    void setEndPoint(const QVector3D& pt);

    QColor color() const;
    void setColor(const QColor& color);
    QColor getColor() const;

    double lineWidth() const;
    void setLineWidth(double width);
    bool isVisible() const;
    void setVisibility(bool visible);
    double getOpacity() const;
    void setOpacity(double opacity);
    QString renderMode() const;
    void setRenderMode(const QString& mode);

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;

private:
    QVector3D startPoint_;
    QVector3D endPoint_;
    QColor color_{Qt::cyan};
    double lineWidth_{2.0};
    bool visibilityFlag_{true};
    double opacityValue_{1.0};
    QString renderMode_{"wireframe"};
};
