// 文件说明：参数模块界面协调器声明，负责参数表单展示与输入回传。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

#pragma once
#include "../../ui/coordination/ModuleCoordinator.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QHash>

/// 参数模块界面协调器。
///
/// 负责组织参数编辑表单，并将用户输入转发给逻辑层。
class ParamsModuleCoordinator : public ModuleCoordinator {
    Q_OBJECT
public:
    // --- 模块基础接口 ---
    explicit ParamsModuleCoordinator(QObject* parent = nullptr);

    QString moduleName() const override;
    QWidget* mainPage() override;
    void onModuleNotification(const LogicNotification& notification) override;
    void activate() override;

private slots:
    void onParamEdited(const QString& key, const QString& value);

private:
    // --- 界面内部装配 ---
    void buildUi();
    void addParamField(const QString& key, const QString& defaultValue = QString());

    QWidget* page_{nullptr};                    // 模块主页。
    QWidget* rightPanel_{nullptr};              // 右侧参数说明区域。
    QFormLayout* formLayout_{nullptr};          // 参数表单布局。
    QHash<QString, QLineEdit*> fields_;         // 参数键到输入框的映射。
};
