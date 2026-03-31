#pragma once
#include <QObject>
#include <QHash>
#include <QDialog>

class VtkSceneWindow;

class GlobalUiManager : public QObject {
    Q_OBJECT
public:
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
    QHash<QString, VtkSceneWindow*> windows_;
};
