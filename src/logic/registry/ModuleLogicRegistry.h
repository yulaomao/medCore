// 文件说明：模块逻辑注册表声明，负责保存和检索模块处理器实例。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QObject>
#include <QHash>
#include <QVector>
#include <QSharedPointer>
#include "ModuleLogicHandler.h"

/// 模块逻辑注册表。
///
/// 维护模块名称到处理器实例的映射，并保留遍历所有处理器的顺序列表。
class ModuleLogicRegistry : public QObject {
    Q_OBJECT
public:
    // --- 处理器注册与查询 ---
    explicit ModuleLogicRegistry(QObject* parent = nullptr);

    void registerHandler(QSharedPointer<ModuleLogicHandler> handler);
    QSharedPointer<ModuleLogicHandler> getHandler(const QString& moduleName) const;
    const QVector<QSharedPointer<ModuleLogicHandler>>& allHandlers() const;

private:
    QHash<QString, QSharedPointer<ModuleLogicHandler>> handlers_; // 便于按模块名检索。
    QVector<QSharedPointer<ModuleLogicHandler>> handlerList_;     // 保留注册顺序，便于遍历。
};
