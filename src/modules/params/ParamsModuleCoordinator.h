#pragma once
#include "../../ui/coordination/ModuleCoordinator.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QHash>

class ParamsModuleCoordinator : public ModuleCoordinator {
    Q_OBJECT
public:
    explicit ParamsModuleCoordinator(QObject* parent = nullptr);

    QString moduleName() const override;
    QWidget* mainPage() override;
    void onModuleNotification(const LogicNotification& notification) override;
    void activate() override;

private slots:
    void onParamEdited(const QString& key, const QString& value);

private:
    void buildUi();
    void addParamField(const QString& key, const QString& defaultValue = QString());

    QWidget* page_{nullptr};
    QFormLayout* formLayout_{nullptr};
    QHash<QString, QLineEdit*> fields_;
};
