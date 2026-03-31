#pragma once
#include <QObject>
#include <QUuid>
#include <QString>
#include <QHash>
#include <QJsonObject>

struct DisplayTarget {
    bool visible{true};
    int layer{0}; // 0=model, 1=overlay, 2=top
};

class NodeBase : public QObject {
    Q_OBJECT
    Q_PROPERTY(QUuid nodeId READ nodeId CONSTANT)
    Q_PROPERTY(QString nodeType READ nodeType CONSTANT)

public:
    explicit NodeBase(const QString& nodeType, QObject* parent = nullptr);
    ~NodeBase() override = default;

    QUuid nodeId() const;
    QString nodeType() const;

    DisplayTarget defaultDisplayTarget() const;
    void setDefaultDisplayTarget(const DisplayTarget& target);

    DisplayTarget displayTargetForWindow(const QString& windowId) const;
    void setWindowDisplayOverride(const QString& windowId, const DisplayTarget& target);
    void removeWindowDisplayOverride(const QString& windowId);
    QHash<QString, DisplayTarget> windowDisplayOverrides() const;

    void beginModify();
    void endModify();

    virtual QJsonObject toJson() const;
    virtual void fromJson(const QJsonObject& obj);

signals:
    void modified(const QString& nodeId);

protected:
    void markDirty();

    QUuid nodeId_;
    QString nodeType_;
    DisplayTarget defaultDisplayTarget_;
    QHash<QString, DisplayTarget> windowDisplayOverrides_;

private:
    int modifyDepth_{0};
    bool dirty_{false};
};
