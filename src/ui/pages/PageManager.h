#pragma once
#include <QObject>
#include <QHash>
#include <QStackedWidget>

class PageManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentPageId READ currentPageId NOTIFY pageChanged)
public:
    explicit PageManager(QStackedWidget* stack, QObject* parent = nullptr);

    void registerPage(const QString& pageId, QWidget* page);
    void showPage(const QString& pageId);
    QString currentPageId() const;

    QStackedWidget* stackedWidget() const;

signals:
    void pageChanged(const QString& oldPage, const QString& newPage);

private:
    QStackedWidget* stack_;
    QHash<QString, QWidget*> pages_;
    QString currentPageId_;
};
