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

public:
    explicit PointNode(QObject* parent = nullptr);

    QVector3D position() const;
    void setPosition(const QVector3D& pos);

    QString label() const;
    void setLabel(const QString& label);

    QColor color() const;
    void setColor(const QColor& color);

    double radius() const;
    void setRadius(double radius);

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;

private:
    QVector3D position_;
    QString label_;
    QColor color_{Qt::yellow};
    double radius_{3.0};
};
