// 文件说明：模块协调器基类声明，统一模块主页、附件区域与动作发送接口。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include <QObject>
#include <QPointer>
#include <QWidget>
#include "../../logic/gateway/ILogicGateway.h"

/// 模块界面协调器基类。
///
/// 负责约束各模块主页、附件面板和界面动作发送方式。
class ModuleCoordinator : public QObject {
    Q_OBJECT
public:
    /// 附件区域位置。
    /// Right 表示右侧面板，Bottom 表示底部面板。
    enum class AttachmentSlot {
        Right,
        Bottom
    };

    explicit ModuleCoordinator(QObject* parent = nullptr);
    ~ModuleCoordinator() override = default;

    // --- 模块标识与页面入口 ---
    virtual QString moduleName() const = 0;
    virtual QWidget* mainPage() = 0;

    // --- 生命周期与通知处理 ---
    virtual void onModuleNotification(const LogicNotification& notification);
    virtual void activate();
    virtual void deactivate();

    void setGateway(ILogicGateway* gateway);
    QWidget* attachmentWidget(AttachmentSlot slot) const;

protected:
    // --- 子类可复用的界面辅助接口 ---
    void sendAction(ActionType type, const QJsonObject& payload);
    void setAttachmentWidget(AttachmentSlot slot, QWidget* widget);

    ILogicGateway* gateway_{nullptr};          // 逻辑网关。
    QPointer<QWidget> rightAttachment_;        // 右侧附件区域。
    QPointer<QWidget> bottomAttachment_;       // 底部附件区域。
};
