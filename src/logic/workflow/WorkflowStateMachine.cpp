#include "WorkflowStateMachine.h"

WorkflowStateMachine::WorkflowStateMachine(const QStringList& workflowSequence,
                                             const QString& initialModule,
                                             QObject* parent)
    : QObject(parent)
    , workflowSequence_(workflowSequence)
    , initialModule_(initialModule)
    , currentModule_(initialModule)
    , enterableModules_(workflowSequence)
{}

QString WorkflowStateMachine::currentModule() const { return currentModule_; }

bool WorkflowStateMachine::canEnter(const QString& module) const {
    if (enterableModules_.contains(module)) return true;
    // Allow forward traversal in workflow sequence
    int currentIdx = workflowSequence_.indexOf(currentModule_);
    int targetIdx  = workflowSequence_.indexOf(module);
    if (currentIdx >= 0 && targetIdx >= 0 && targetIdx >= currentIdx) return true;
    return false;
}

bool WorkflowStateMachine::advance(const QString& targetModule) {
    if (!canEnter(targetModule)) return false;
    const QString old = currentModule_;
    currentModule_ = targetModule;
    emit moduleChanged(old, currentModule_);
    return true;
}

void WorkflowStateMachine::setEnterableModules(const QStringList& modules) {
    enterableModules_ = modules;
}

QStringList WorkflowStateMachine::enterableModules() const { return enterableModules_; }

void WorkflowStateMachine::reset() {
    const QString old = currentModule_;
    currentModule_ = initialModule_;
    if (old != currentModule_) emit moduleChanged(old, currentModule_);
}
