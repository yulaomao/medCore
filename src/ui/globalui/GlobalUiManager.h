// 文件说明：全局界面管理器声明，负责提示消息、模态框与三维窗口注册。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QObject>
#include <QHash>
#include <QDialog>

class VtkSceneWindow;

/// 全局界面管理器。
///
/// 负责统一弹出提示、模态对话框和跨模块三维窗口注册表。
class GlobalUiManager : public QObject {
    Q_OBJECT
public:
    // --- 全局提示与窗口注册 ---
    explicit GlobalUiManager(QObject* parent = nullptr);
    ~GlobalUiManager() override = default;

    void showToast(const QString& message, int durationMs = 3000);
    int  showModal(const QString& title, const QString& message);

    void register3dWindow(const QString& windowId, VtkSceneWindow* window);
    void unregister3dWindow(const QString& windowId);
    VtkSceneWindow* getWindow(const QString& windowId) const;

signals:
    void toastRequested(const QString& message, int durationMs);

private:
    QHash<QString, VtkSceneWindow*> windows_;   // 已注册的三维窗口索引。
};
