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

struct CameraParameters {
    double position[3]{0.0, 0.0, 100.0};
    double focalPoint[3]{0.0, 0.0, 0.0};
    double viewUp[3]{0.0, 1.0, 0.0};
    bool parallelProjection{false};
    double parallelScale{1.0};
    double viewAngle{30.0};
    double clippingRange[2]{0.1, 1000.0};
};

class VtkSceneWindow : public QVTKOpenGLNativeWidget {
    Q_OBJECT
    Q_PROPERTY(QString windowId READ windowId CONSTANT)
public:
    explicit VtkSceneWindow(const QString& windowId, QWidget* parent = nullptr);
    ~VtkSceneWindow() override = default;

    QString windowId() const;

    void setInitialCameraParameters(const CameraParameters& params);
    CameraParameters initialCameraParameters() const;

    void updateNodes(const QVector<NodeBase*>& nodes);

public slots:
    void resetCamera();
    void onInteraction();

private:
    vtkSmartPointer<vtkActor> createActorForNode(NodeBase* node);
    void updateActorFromNode(vtkActor* actor, NodeBase* node);
    void applyCameraParameters(const CameraParameters& params);

    QString windowId_;

    vtkSmartPointer<vtkRenderer> modelRenderer_;
    vtkSmartPointer<vtkRenderer> overlayRenderer_;
    vtkSmartPointer<vtkRenderer> topRenderer_;
    vtkSmartPointer<vtkCamera>   sharedCamera_;

    QHash<QString, vtkSmartPointer<vtkActor>> actorCache_;
    QTimer cameraResetTimer_;
    CameraParameters initialCameraParams_;
};
