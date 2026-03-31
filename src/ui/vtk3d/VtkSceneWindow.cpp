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

        // Map guide layers 1/2/3 to VTK render layers model/overlay/top.
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

    // Remove stale actors
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
        // Placeholder: create a unit cube actor when no file is loaded
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
        actor->GetProperty()->SetRepresentationToWireframe();
        if (mdl->renderMode() == RENDER_MODE_POINTS) {
            actor->GetProperty()->SetRepresentationToPoints();
        } else if (mdl->renderMode() != RENDER_MODE_WIREFRAME) {
            actor->GetProperty()->SetRepresentationToSurface();
        }
    }
}

void VtkSceneWindow::resetCamera() {
    modelRenderer_->ResetCamera();
    renderWindow()->Render();
}

void VtkSceneWindow::onInteraction() {
    cameraResetTimer_.start(); // restart 3-second timer
}
