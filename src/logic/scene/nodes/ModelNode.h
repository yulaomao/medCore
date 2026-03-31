#pragma once
#include "NodeBase.h"
#include <QColor>

class ModelNode : public NodeBase {
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(double opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(bool wireframe READ wireframe WRITE setWireframe)
    Q_PROPERTY(bool visibilityFlag READ isVisible WRITE setVisibility)
    Q_PROPERTY(QString renderMode READ renderMode WRITE setRenderMode)

public:
    explicit ModelNode(QObject* parent = nullptr);

    QString filePath() const;
    void setFilePath(const QString& path);

    QColor color() const;
    void setColor(const QColor& color);
    QColor getColor() const;

    double opacity() const;
    void setOpacity(double opacity);
    double getOpacity() const;
    bool isVisible() const;
    void setVisibility(bool visible);

    bool wireframe() const;
    void setWireframe(bool wf);
    QString renderMode() const;
    void setRenderMode(const QString& mode);

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;

private:
    QString filePath_;
    QColor color_{Qt::white};
    double opacity_{1.0};
    bool visibilityFlag_{true};
    bool wireframe_{false};
    QString renderMode_{"surface"};
};
