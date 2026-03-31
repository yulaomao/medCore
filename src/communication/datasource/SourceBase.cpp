// 文件说明：实现数据源基类的通用状态管理、启动停止与信号发射流程。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "SourceBase.h"

SourceBase::SourceBase(const QString& channelName, QObject* parent)
    : QObject(parent)
    , channelName_(channelName)
{}

QString SourceBase::channelName() const { return channelName_; }

QString SourceBase::lastError() const { return lastError_; }

void SourceBase::reportError(const QString& errorMessage) {
    lastError_ = errorMessage;
    emit errorOccurred(channelName_, errorMessage);
}
