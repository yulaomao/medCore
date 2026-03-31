// 文件说明：软件初始化器工厂声明，负责按软件类型创建初始化器。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QSharedPointer>
#include <QString>
#include "BaseSoftwareInitializer.h"

/// 软件初始化器工厂。
///
/// 根据外部解析得到的软件类型创建对应的初始化器对象。
class SoftwareInitializerFactory {
public:
    // 根据软件类型创建具体初始化器；未知类型回退到默认实现。
    static QSharedPointer<BaseSoftwareInitializer> create(const QString& softwareType);

private:
    SoftwareInitializerFactory() = delete;
};
