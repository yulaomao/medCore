#pragma once
#include <QObject>
#include <QHash>
#include <QSharedPointer>
#include "ModuleCoordinator.h"
#include "../pages/PageManager.h"
#include "../globalui/GlobalUiManager.h"

class MainWindow;

class ApplicationCoordinator : public QObject {
    Q_OBJECT
public:
    explicit ApplicationCoordinator(ILogicGateway* gateway,
                                     PageManager* pageManager,
                                     GlobalUiManager* uiManager,
                                     QObject* parent = nullptr);

    void registerModuleCoordinator(QSharedPointer<ModuleCoordinator> coordinator);
    void activateModule(const QString& moduleName);
    void connectShellSignals(MainWindow* mainWindow);

signals:
    void moduleActivated(const QString& moduleName);

public slots:
    void onNotification(const LogicNotification& notification);

private:
    ILogicGateway* gateway_;
    PageManager* pageManager_;
    GlobalUiManager* uiManager_;
    QHash<QString, QSharedPointer<ModuleCoordinator>> coordinators_;
    QString activeModule_;
};
