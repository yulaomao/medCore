// 文件说明：应用级协调器声明，负责模块注册、激活与通知分发。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QObject>
#include <QHash>
#include <QSharedPointer>
#include "ModuleCoordinator.h"
#include "../pages/PageManager.h"
#include "../globalui/GlobalUiManager.h"

class MainWindow;
class WorkspaceShell;

/// 应用级协调器。
///
/// 统一管理模块协调器、页面切换、界面提示和工作区附件挂载。
class ApplicationCoordinator : public QObject {
    Q_OBJECT
public:
    // --- 模块注册与激活 ---
    explicit ApplicationCoordinator(ILogicGateway* gateway,
                                     PageManager* pageManager,
                                     GlobalUiManager* uiManager,
                                     QObject* parent = nullptr);

    void registerModuleCoordinator(QSharedPointer<ModuleCoordinator> coordinator);
    void activateModule(const QString& moduleName);
    void connectShellSignals(MainWindow* mainWindow);
    void requestModuleActivation(const QString& moduleName);

signals:
    void moduleActivated(const QString& moduleName);

public slots:
    void onNotification(const LogicNotification& notification);
    void onConnectionStateChanged(const QString& state);

private:
    // --- 内部协调流程 ---
    void mountModuleAttachments(const QString& moduleName);

    ILogicGateway* gateway_;                                     // 逻辑网关。
    PageManager* pageManager_;                                   // 页面管理器。
    GlobalUiManager* uiManager_;                                 // 全局界面管理器。
    QHash<QString, QSharedPointer<ModuleCoordinator>> coordinators_; // 模块名到协调器的映射。
    QString activeModule_;                                       // 当前激活模块。
    WorkspaceShell* workspaceShell_{nullptr};                    // 当前连接的工作区外壳。
};
