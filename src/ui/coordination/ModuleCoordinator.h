#pragma once
#include <QObject>
#include <QPointer>
#include <QWidget>
#include "../../logic/gateway/ILogicGateway.h"

class ModuleCoordinator : public QObject {
    Q_OBJECT
public:
    enum class AttachmentSlot {
        Right,
        Bottom
    };

    explicit ModuleCoordinator(QObject* parent = nullptr);
    ~ModuleCoordinator() override = default;

    virtual QString moduleName() const = 0;
    virtual QWidget* mainPage() = 0;

    virtual void onModuleNotification(const LogicNotification& notification);
    virtual void activate();
    virtual void deactivate();

    void setGateway(ILogicGateway* gateway);
    QWidget* attachmentWidget(AttachmentSlot slot) const;

protected:
    void sendAction(ActionType type, const QJsonObject& payload);
    void setAttachmentWidget(AttachmentSlot slot, QWidget* widget);

    ILogicGateway* gateway_{nullptr};
    QPointer<QWidget> rightAttachment_;
    QPointer<QWidget> bottomAttachment_;
};
