// 文件说明：实现 VTK 三维场景窗口，负责节点渲染、分层显示与相机复位。
// 该文件属于 medCore 当前主工程源码范围，用于承载对应模块的核心实现。

#include "VtkSceneWindow.h"
#include <QSet>
#include "../../logic/scene/nodes/NodeBase.h"
#include "../../logic/scene/nodes/PointNode.h"
#include "../../logic/scene/nodes/LineNode.h"
#include "../../logic/scene/nodes/ModelNode.h"

#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCallbackCommand.h>

static constexpr int CAMERA_RESET_DELAY_MS = 3000;
static constexpr const char* RENDER_MODE_SURFACE = "surface";
static constexpr const char* RENDER_MODE_WIREFRAME = "wireframe";
static constexpr const char* RENDER_MODE_POINTS = "points";

static void interactionCallback(vtkObject* /*caller*/, unsigned long /*eid*/,
                                 void* clientData, void* /*callData*/)
{
    auto* win = static_cast<VtkSceneWindow*>(clientData);
    if (win) win->onInteraction();
}

VtkSceneWindow::VtkSceneWindow(const QString& windowId, QWidget* parent)
    : QVTKOpenGLNativeWidget(parent)
    , windowId_(windowId)
{
    auto renWin = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    setRenderWindow(renWin);

    sharedCamera_ = vtkSmartPointer<vtkCamera>::New();

    modelRenderer_   = vtkSmartPointer<vtkRenderer>::New();
    overlayRenderer_ = vtkSmartPointer<vtkRenderer>::New();
    topRenderer_     = vtkSmartPointer<vtkRenderer>::New();

    modelRenderer_->SetLayer(0);
    overlayRenderer_->SetLayer(1);
    topRenderer_->SetLayer(2);

    modelRenderer_->SetActiveCamera(sharedCamera_);
    overlayRenderer_->SetActiveCamera(sharedCamera_);
    topRenderer_->SetActiveCamera(sharedCamera_);

    modelRenderer_->SetBackground(0.1, 0.1, 0.15);
    overlayRenderer_->SetBackground2(0.0, 0.0, 0.0);
    overlayRenderer_->SetBackgroundAlpha(0.0);
    overlayRenderer_->InteractiveOff();
    topRenderer_->SetBackgroundAlpha(0.0);
    topRenderer_->InteractiveOff();

    renWin->SetNumberOfLayers(3);
    renWin->AddRenderer(modelRenderer_);
    renWin->AddRenderer(overlayRenderer_);
    renWin->AddRenderer(topRenderer_);

    auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    renWin->GetInteractor()->SetInteractorStyle(style);

    auto cb = vtkSmartPointer<vtkCallbackCommand>::New();
    cb->SetCallback(interactionCallback);
    cb->SetClientData(this);
    renWin->GetInteractor()->AddObserver(vtkCommand::MouseMoveEvent, cb);
    renWin->GetInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent, cb);

    cameraResetTimer_.setSingleShot(true);
    cameraResetTimer_.setInterval(CAMERA_RESET_DELAY_MS);
    connect(&cameraResetTimer_, &QTimer::timeout, this, &VtkSceneWindow::resetCamera);

    // 应用默认相机参数
    applyCameraParameters(initialCameraParams_);
}

QString VtkSceneWindow::windowId() const { return windowId_; }

void VtkSceneWindow::updateNodes(const QVector<NodeBase*>& nodes) {
    QSet<QString> activeIds;

    for (NodeBase* node : nodes) {
        if (!node) continue;
        const QString id = node->nodeId().toString();
        activeIds.insert(id);

        DisplayTarget dt = node->displayTargetForWindow(windowId_);
        if (!dt.visible) {
            actorCache_.remove(id);
            continue;
        }

        vtkSmartPointer<vtkActor> actor;
        if (actorCache_.contains(id)) {
            actor = actorCache_[id];
            updateActorFromNode(actor, node);
        } else {
            actor = createActorForNode(node);
            if (!actor) continue;
            actorCache_[id] = actor;
        }

        // 将引导层 1/2/3 映射到 VTK 的 model/overlay/top 三个渲染层
        vtkRenderer* target = modelRenderer_;
        if (dt.layer == 2) target = overlayRenderer_;
        else if (dt.layer == 3) target = topRenderer_;

        if (!target->HasViewProp(actor)) {
            modelRenderer_->RemoveActor(actor);
            overlayRenderer_->RemoveActor(actor);
            topRenderer_->RemoveActor(actor);
            target->AddActor(actor);
        }
    }

    // 清理已经失效的演员对象
    for (auto it = actorCache_.begin(); it != actorCache_.end(); ) {
        if (!activeIds.contains(it.key())) {
            modelRenderer_->RemoveActor(it.value());
            overlayRenderer_->RemoveActor(it.value());
            topRenderer_->RemoveActor(it.value());
            it = actorCache_.erase(it);
        } else {
            ++it;
        }
    }

    renderWindow()->Render();
}

vtkSmartPointer<vtkActor> VtkSceneWindow::createActorForNode(NodeBase* node) {
    auto actor = vtkSmartPointer<vtkActor>::New();
    updateActorFromNode(actor, node);
    return actor;
}

void VtkSceneWindow::updateActorFromNode(vtkActor* actor, NodeBase* node) {
    if (!actor || !node) return;

    if (auto* pt = qobject_cast<PointNode*>(node)) {
        // 简化渲染时仅使用首个点，后续可扩展为真正的多点渲染
        auto sphere = vtkSmartPointer<vtkSphereSource>::New();
        sphere->SetCenter(pt->position().x(), pt->position().y(), pt->position().z());
        sphere->SetRadius(pt->radius());
        sphere->SetPhiResolution(16);
        sphere->SetThetaResolution(16);
        sphere->Update();
        auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputData(sphere->GetOutput());
        actor->SetMapper(mapper);
        QColor c = pt->getColor();
        actor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());
        actor->GetProperty()->SetOpacity(pt->getOpacity());
        actor->SetVisibility(pt->isVisible());
    } else if (auto* ln = qobject_cast<LineNode*>(node)) {
        // 处理折线时先使用首尾顶点做简化的两点渲染
        auto line = vtkSmartPointer<vtkLineSource>::New();
        line->SetPoint1(ln->startPoint().x(), ln->startPoint().y(), ln->startPoint().z());
        line->SetPoint2(ln->endPoint().x(), ln->endPoint().y(), ln->endPoint().z());
        line->Update();
        auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputData(line->GetOutput());
        actor->SetMapper(mapper);
        QColor c = ln->getColor();
        actor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());
        actor->GetProperty()->SetLineWidth(static_cast<float>(ln->lineWidth()));
        actor->GetProperty()->SetOpacity(ln->getOpacity());
        actor->SetVisibility(ln->isVisible());
    } else if (auto* mdl = qobject_cast<ModelNode*>(node)) {
        // 占位实现：在尚未加载真实网格时先创建一个球体演员
        auto sphere = vtkSmartPointer<vtkSphereSource>::New();
        sphere->SetRadius(5.0);
        sphere->Update();
        auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputData(sphere->GetOutput());
        actor->SetMapper(mapper);
        QColor c = mdl->getColor();
        actor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());
        actor->GetProperty()->SetOpacity(mdl->getOpacity());
        actor->SetVisibility(mdl->isVisible());
        // 渲染模式
        if (mdl->renderMode() == RENDER_MODE_POINTS) {
            actor->GetProperty()->SetRepresentationToPoints();
        } else if (mdl->renderMode() == RENDER_MODE_WIREFRAME) {
            actor->GetProperty()->SetRepresentationToWireframe();
        } else {
            actor->GetProperty()->SetRepresentationToSurface();
        }
        // 边缘显示
        actor->GetProperty()->SetEdgeVisibility(mdl->isShowEdges());
        if (mdl->isShowEdges()) {
            QColor ec = mdl->getEdgeColor();
            actor->GetProperty()->SetEdgeColor(ec.redF(), ec.greenF(), ec.blueF());
            actor->GetProperty()->SetLineWidth(static_cast<float>(mdl->getEdgeWidth()));
        }
        // 背面剔除
        actor->GetProperty()->SetBackfaceCulling(mdl->isBackfaceCulling());
    }
}

void VtkSceneWindow::setInitialCameraParameters(const CameraParameters& params) {
    initialCameraParams_ = params;
    applyCameraParameters(params);
    renderWindow()->Render();
}

CameraParameters VtkSceneWindow::initialCameraParameters() const {
    return initialCameraParams_;
}

void VtkSceneWindow::applyCameraParameters(const CameraParameters& params) {
    if (!sharedCamera_) return;
    sharedCamera_->SetPosition(params.position);
    sharedCamera_->SetFocalPoint(params.focalPoint);
    sharedCamera_->SetViewUp(params.viewUp);
    sharedCamera_->SetClippingRange(params.clippingRange);
    if (params.parallelProjection) {
        sharedCamera_->ParallelProjectionOn();
        sharedCamera_->SetParallelScale(params.parallelScale);
    } else {
        sharedCamera_->ParallelProjectionOff();
        sharedCamera_->SetViewAngle(params.viewAngle);
    }
}

void VtkSceneWindow::resetCamera() {
    // 恢复为初始相机参数，对应设计 13.2 的界面约束 5
    applyCameraParameters(initialCameraParams_);
    renderWindow()->Render();
}

void VtkSceneWindow::onInteraction() {
    cameraResetTimer_.start(); // 重新开始 3 秒计时
}
