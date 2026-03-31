// 文件说明：实现全局界面管理器，统一管理三维窗口与全局提示。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "GlobalUiManager.h"
#include "../vtk3d/VtkSceneWindow.h"
#include <QMessageBox>

GlobalUiManager::GlobalUiManager(QObject* parent) : QObject(parent) {}

void GlobalUiManager::showToast(const QString& message, int durationMs) {
    emit toastRequested(message, durationMs);
}

int GlobalUiManager::showModal(const QString& title, const QString& message) {
    QMessageBox box;
    box.setWindowTitle(title);
    box.setText(message);
    box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    return box.exec();
}

void GlobalUiManager::register3dWindow(const QString& windowId, VtkSceneWindow* window) {
    windows_[windowId] = window;
}

void GlobalUiManager::unregister3dWindow(const QString& windowId) {
    windows_.remove(windowId);
}

VtkSceneWindow* GlobalUiManager::getWindow(const QString& windowId) const {
    return windows_.value(windowId, nullptr);
}
