#pragma once
#include "../../logic/registry/ModuleLogicHandler.h"
#include "../../logic/scene/SceneGraph.h"
#include "../../logic/scene/nodes/TransformNode.h"
#include <QSharedPointer>

class NavigationModuleHandler : public ModuleLogicHandler {
    Q_OBJECT
public:
    explicit NavigationModuleHandler(SceneGraph* sceneGraph, QObject* parent = nullptr);

    QString moduleName() const override;
    void onAction(const UiAction& action) override;
    void onStateSample(const QString& channel, const QJsonObject& data) override;

private:
    SceneGraph* sceneGraph_;
    QSharedPointer<TransformNode> poseNode_;
    QString poseNodeId_;
};
