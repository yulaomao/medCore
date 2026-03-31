#pragma once
#include "../../logic/registry/ModuleLogicHandler.h"
#include "../../logic/scene/SceneGraph.h"

class ParamsModuleHandler : public ModuleLogicHandler {
    Q_OBJECT
public:
    explicit ParamsModuleHandler(SceneGraph* sceneGraph, QObject* parent = nullptr);

    QString moduleName() const override;
    void onAction(const UiAction& action) override;
    void onStateSample(const QString& channel, const QJsonObject& data) override;

private:
    SceneGraph* sceneGraph_;
    QJsonObject currentParams_;
};
