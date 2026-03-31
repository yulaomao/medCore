// 文件说明：VTK 场景窗口声明，负责节点渲染、相机控制与演员缓存。
// 本头文件用于描述对应模块的类型声明、函数接口和关键成员变量语义。

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

/// 相机参数集合。
///
/// 用于保存三维窗口的初始观察位置、投影方式和裁剪范围。
struct CameraParameters {
    double position[3]{0.0, 0.0, 100.0};        // 相机位置。
    double focalPoint[3]{0.0, 0.0, 0.0};        // 相机焦点。
    double viewUp[3]{0.0, 1.0, 0.0};            // 相机向上方向。
    bool parallelProjection{false};             // 是否启用平行投影。
    double parallelScale{1.0};                  // 平行投影缩放。
    double viewAngle{30.0};                     // 透视投影视角。
    double clippingRange[2]{0.1, 1000.0};      // 近平面与远平面裁剪距离。
};

/// VTK 三维场景窗口。
///
/// 使用三层渲染器和共享相机显示场景节点，并缓存演员对象以减少重复构建。
class VtkSceneWindow : public QVTKOpenGLNativeWidget {
    Q_OBJECT
    Q_PROPERTY(QString windowId READ windowId CONSTANT)
public:
    // --- 窗口基础信息与相机配置 ---
    explicit VtkSceneWindow(const QString& windowId, QWidget* parent = nullptr);
    ~VtkSceneWindow() override = default;

    QString windowId() const;

    void setInitialCameraParameters(const CameraParameters& params);
    CameraParameters initialCameraParameters() const;

    // 将外部节点列表同步为当前窗口中的演员集合。
    void updateNodes(const QVector<NodeBase*>& nodes);

public slots:
    void resetCamera();
    void onInteraction();

private:
    vtkSmartPointer<vtkActor> createActorForNode(NodeBase* node);
    void updateActorFromNode(vtkActor* actor, NodeBase* node);
    void applyCameraParameters(const CameraParameters& params);

    QString windowId_;                                  // 窗口唯一标识。

    vtkSmartPointer<vtkRenderer> modelRenderer_;        // 基础模型层渲染器。
    vtkSmartPointer<vtkRenderer> overlayRenderer_;      // 中间叠加层渲染器。
    vtkSmartPointer<vtkRenderer> topRenderer_;          // 顶层引导元素渲染器。
    vtkSmartPointer<vtkCamera>   sharedCamera_;         // 三层共享的相机。

    QHash<QString, vtkSmartPointer<vtkActor>> actorCache_; // 节点到演员对象的缓存。
    QTimer cameraResetTimer_;                           // 交互停止后的相机复位计时器。
    CameraParameters initialCameraParams_;              // 初始相机参数。
};
