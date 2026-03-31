// 文件说明：页面管理器声明，负责注册页面并在堆栈容器中切换。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QObject>
#include <QHash>
#include <QStackedWidget>

/// 页面管理器。
///
/// 持有页面标识与页面实例的映射，并驱动 QStackedWidget 页面切换。
class PageManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentPageId READ currentPageId NOTIFY pageChanged)
public:
    // --- 页面注册与切换 ---
    explicit PageManager(QStackedWidget* stack, QObject* parent = nullptr);

    void registerPage(const QString& pageId, QWidget* page);
    void showPage(const QString& pageId);
    QString currentPageId() const;

    QStackedWidget* stackedWidget() const;

signals:
    void pageChanged(const QString& oldPage, const QString& newPage);

private:
    QStackedWidget* stack_;            // 页面堆栈容器。
    QHash<QString, QWidget*> pages_;   // 页面标识到页面对象的映射。
    QString currentPageId_;            // 当前显示的页面标识。
};
