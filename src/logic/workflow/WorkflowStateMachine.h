// 文件说明：工作流状态机声明，负责模块切换顺序与可进入权限控制。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QObject>
#include <QStringList>

/// 工作流状态机。
///
/// 保存模块执行顺序、当前模块和可进入模块集合，并发出切换通知。
class WorkflowStateMachine : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentModule READ currentModule NOTIFY moduleChanged)
public:
    // --- 工作流查询与切换 ---
    explicit WorkflowStateMachine(const QStringList& workflowSequence,
                                   const QString& initialModule,
                                   QObject* parent = nullptr);

    QString currentModule() const;
    QStringList workflowSequence() const;
    bool canEnter(const QString& module) const;
    bool advance(const QString& targetModule);

    void setEnterableModules(const QStringList& modules);
    QStringList enterableModules() const;

    void reset();

signals:
    void moduleChanged(const QString& oldModule, const QString& newModule);

private:
    QStringList workflowSequence_;   // 工作流顺序定义。
    QString initialModule_;          // 初始模块。
    QString currentModule_;          // 当前模块。
    QStringList enterableModules_;   // 当前允许进入的模块集合。
};
