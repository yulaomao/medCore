#pragma once
#include "../../logic/registry/ModuleLogicHandler.h"
#include "../../logic/scene/SceneGraph.h"
#include "../../logic/scene/nodes/PointNode.h"
#include <QHash>

class PointPickModuleHandler : public ModuleLogicHandler {
    Q_OBJECT
public:
    explicit PointPickModuleHandler(SceneGraph* sceneGraph, QObject* parent = nullptr);

    QString moduleName() const override;
    void onAction(const UiAction& action) override;
    void onStateSample(const QString& channel, const QJsonObject& data) override;

private:
    SceneGraph* sceneGraph_;
    QHash<QString, QSharedPointer<PointNode>> managedPoints_;

    void addPoint(const QJsonObject& params, const QUuid& sourceActionId);
    void removePoint(const QString& pointId, const QUuid& sourceActionId);
    void clearPoints(const QUuid& sourceActionId);
};
