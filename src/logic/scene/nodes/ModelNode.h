#pragma once
#include "NodeBase.h"
#include <QColor>

class ModelNode : public NodeBase {
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(double opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(bool wireframe READ wireframe WRITE setWireframe)

public:
    explicit ModelNode(QObject* parent = nullptr);

    QString filePath() const;
    void setFilePath(const QString& path);

    QColor color() const;
    void setColor(const QColor& color);

    double opacity() const;
    void setOpacity(double opacity);

    bool wireframe() const;
    void setWireframe(bool wf);

    QJsonObject toJson() const override;
    void fromJson(const QJsonObject& obj) override;

private:
    QString filePath_;
    QColor color_{Qt::white};
    double opacity_{1.0};
    bool wireframe_{false};
};
