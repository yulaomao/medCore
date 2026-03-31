// 文件说明：实现页面管理器，维护页面注册、切换与查询。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "PageManager.h"
#include <QDebug>

PageManager::PageManager(QStackedWidget* stack, QObject* parent)
    : QObject(parent)
    , stack_(stack)
{}

void PageManager::registerPage(const QString& pageId, QWidget* page) {
    if (!page) return;
    pages_[pageId] = page;
    stack_->addWidget(page);
}

void PageManager::showPage(const QString& pageId) {
    if (!pages_.contains(pageId)) {
        qWarning() << "PageManager: unknown pageId" << pageId;
        return;
    }
    const QString old = currentPageId_;
    currentPageId_ = pageId;
    stack_->setCurrentWidget(pages_[pageId]);
    if (old != currentPageId_) emit pageChanged(old, currentPageId_);
}

QString PageManager::currentPageId() const { return currentPageId_; }

QStackedWidget* PageManager::stackedWidget() const { return stack_; }
