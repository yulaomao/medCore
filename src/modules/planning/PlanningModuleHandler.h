#pragma once
#include "../../logic/registry/ModuleLogicHandler.h"
#include "../../logic/scene/SceneGraph.h"
#include "../../logic/scene/nodes/LineNode.h"
#include "../../logic/scene/nodes/ModelNode.h"
#include <QHash>

class PlanningModuleHandler : public ModuleLogicHandler {
    Q_OBJECT
public:
    explicit PlanningModuleHandler(SceneGraph* sceneGraph, QObject* parent = nullptr);

    QString moduleName() const override;
    void onAction(const UiAction& action) override;
    void onStateSample(const QString& channel, const QJsonObject& data) override;

private:
    SceneGraph* sceneGraph_;
    QHash<QString, QSharedPointer<LineNode>>  managedLines_;
    QHash<QString, QSharedPointer<ModelNode>> managedModels_;

    void loadModel(const QJsonObject& params, const QUuid& sourceActionId);
    void addLine(const QJsonObject& params, const QUuid& sourceActionId);
    void removeLine(const QString& lineId, const QUuid& sourceActionId);
};
