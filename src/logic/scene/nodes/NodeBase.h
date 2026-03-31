#pragma once
#include <QObject>
#include <QUuid>
#include <QString>
#include <QHash>
#include <QStringList>
#include <QJsonObject>
#include <QJsonValue>

struct DisplayTarget {
    bool visible{true};
    int layer{1};
};

enum class NodeEventType {
    NodeModified,
    PayloadModified,
    ReferenceChanged,
    TransformChanged
};

class NodeBase : public QObject {
    Q_OBJECT
    Q_PROPERTY(QUuid nodeId READ nodeId CONSTANT)
    Q_PROPERTY(QString nodeTagName READ nodeTagName CONSTANT)
    Q_PROPERTY(QString nodeType READ nodeType CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(int version READ version WRITE setVersion)

public:
    explicit NodeBase(const QString& nodeType, QObject* parent = nullptr);
    ~NodeBase() override = default;

    QUuid nodeId() const;
    QString nodeTagName() const;
    QString nodeType() const;
    QString name() const;
    void setName(const QString& name);
    QString description() const;
    void setDescription(const QString& description);
    int version() const;
    void setVersion(int version);
    void setAttribute(const QString& key, const QJsonValue& value);
    QJsonValue getAttribute(const QString& key,
                            const QJsonValue& defaultValue = QJsonValue()) const;
    void removeAttribute(const QString& key);
    QJsonObject attributeMap() const;
    void addReference(const QString& relation, const QString& nodeId);
    void removeReference(const QString& relation, const QString& nodeId);
    QStringList getReferences(const QString& relation) const;
    QHash<QString, QStringList> referenceMap() const;

    DisplayTarget defaultDisplayTarget() const;
    void setDefaultDisplayTarget(const DisplayTarget& target);

    DisplayTarget getDisplayTargetForWindow(const QString& windowId) const;
    DisplayTarget displayTargetForWindow(const QString& windowId) const;
    void setWindowDisplayTarget(const QString& windowId, const DisplayTarget& target);
    void setWindowDisplayOverride(const QString& windowId, const DisplayTarget& target);
    void removeWindowDisplayOverride(const QString& windowId);
    QHash<QString, DisplayTarget> windowDisplayOverrides() const;

    void startBatchModify();
    void beginModify();
    void touchModified();
    void endBatchModify();
    void endModify();

    virtual QJsonObject toJson() const;
    virtual void fromJson(const QJsonObject& obj);

    bool dirtyFlag() const;

signals:
    void modified(const QString& nodeId);
    void nodeEvent(const QString& nodeId, NodeEventType eventType);

protected:
    void markDirty();
    void emitNodeEvent(NodeEventType eventType);

    QUuid nodeId_;
    QString nodeType_;
    QString name_;
    QString description_;
    int version_{1};
    QJsonObject attributeMap_;
    QHash<QString, QStringList> referenceMap_;
    DisplayTarget defaultDisplayTarget_;
    QHash<QString, DisplayTarget> windowDisplayOverrides_;

private:
    int modifyDepth_{0};
    bool dirty_{false};
};
