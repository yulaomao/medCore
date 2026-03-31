// 文件说明：场景节点基类声明，负责公共元数据、引用关系和显示目标管理。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QObject>
#include <QUuid>
#include <QString>
#include <QHash>
#include <QStringList>
#include <QJsonObject>
#include <QJsonValue>

/// 显示目标配置。
///
/// 描述节点在窗口中的可见性以及应落入的引导层。
struct DisplayTarget {
    bool visible{true};   // 是否可见。
    int layer{1};         // 引导层编号，通常取 1~3。
};

/// 节点事件类型。
///
/// 用于标记节点元数据、负载、引用或变换相关的变化来源。
enum class NodeEventType {
    NodeModified,     // 节点基础信息发生变化。
    PayloadModified,  // 负载数据发生变化。
    ReferenceChanged, // 节点引用关系发生变化。
    TransformChanged  // 变换相关数据发生变化。
};

/// 场景节点基类。
///
/// 提供节点身份、属性系统、引用关系、显示目标与序列化等公共能力。
class NodeBase : public QObject {
    Q_OBJECT
    Q_PROPERTY(QUuid nodeId READ nodeId CONSTANT)
    Q_PROPERTY(QString nodeTagName READ nodeTagName CONSTANT)
    Q_PROPERTY(QString nodeType READ nodeType CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(int version READ version WRITE setVersion)

public:
    // --- 节点身份与基础元数据 ---
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

    // --- 可扩展属性与引用关系 ---
    void setAttribute(const QString& key, const QJsonValue& value);
    QJsonValue getAttribute(const QString& key,
                            const QJsonValue& defaultValue = QJsonValue()) const;
    void removeAttribute(const QString& key);
    QJsonObject attributeMap() const;
    void addReference(const QString& relation, const QString& nodeId);
    void removeReference(const QString& relation, const QString& nodeId);
    QStringList getReferences(const QString& relation) const;
    QHash<QString, QStringList> referenceMap() const;

    // --- 默认显示目标与窗口级覆盖 ---
    DisplayTarget defaultDisplayTarget() const;
    void setDefaultDisplayTarget(const DisplayTarget& target);

    DisplayTarget getDisplayTargetForWindow(const QString& windowId) const;
    DisplayTarget displayTargetForWindow(const QString& windowId) const;
    void setWindowDisplayTarget(const QString& windowId, const DisplayTarget& target);
    void setWindowDisplayOverride(const QString& windowId, const DisplayTarget& target);
    void removeWindowDisplayOverride(const QString& windowId);
    QHash<QString, DisplayTarget> windowDisplayOverrides() const;

    // --- 批量修改控制 ---
    void startBatchModify();
    void beginModify();
    void touchModified();
    void endBatchModify();
    void endModify();

    // --- 序列化 ---
    virtual QJsonObject toJson() const;
    virtual void fromJson(const QJsonObject& obj);

    bool dirtyFlag() const;

signals:
    void modified(const QString& nodeId);
    void nodeEvent(const QString& nodeId, NodeEventType eventType);

protected:
    void markDirty();
    void emitNodeEvent(NodeEventType eventType);

    QUuid nodeId_;                                          // 节点唯一标识。
    QString nodeType_;                                      // 节点类型名。
    QString name_;                                          // 节点显示名称。
    QString description_;                                   // 节点说明。
    int version_{1};                                        // 版本号。
    QJsonObject attributeMap_;                              // 扩展属性表。
    QHash<QString, QStringList> referenceMap_;              // 关系名到节点标识列表的映射。
    DisplayTarget defaultDisplayTarget_;                    // 默认显示目标。
    QHash<QString, DisplayTarget> windowDisplayOverrides_;  // 按窗口覆盖的显示目标。

private:
    int modifyDepth_{0};   // 批量修改嵌套深度。
    bool dirty_{false};    // 当前节点是否存在未分发的修改。
};
