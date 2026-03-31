#pragma once
#include <QVTKOpenGLNativeWidget.h>
#include <QString>
#include <QHash>
#include <QTimer>
#include <QVector>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkActor.h>

class NodeBase;

class VtkSceneWindow : public QVTKOpenGLNativeWidget {
    Q_OBJECT
    Q_PROPERTY(QString windowId READ windowId CONSTANT)
public:
    explicit VtkSceneWindow(const QString& windowId, QWidget* parent = nullptr);
    ~VtkSceneWindow() override = default;

    QString windowId() const;

    void updateNodes(const QVector<NodeBase*>& nodes);

public slots:
    void resetCamera();
    void onInteraction();

private:
    vtkSmartPointer<vtkActor> createActorForNode(NodeBase* node);
    void updateActorFromNode(vtkActor* actor, NodeBase* node);

    QString windowId_;

    vtkSmartPointer<vtkRenderer> modelRenderer_;
    vtkSmartPointer<vtkRenderer> overlayRenderer_;
    vtkSmartPointer<vtkRenderer> topRenderer_;
    vtkSmartPointer<vtkCamera>   sharedCamera_;

    QHash<QString, vtkSmartPointer<vtkActor>> actorCache_;
    QTimer cameraResetTimer_;
};
