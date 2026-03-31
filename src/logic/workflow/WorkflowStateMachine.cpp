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
QStringList WorkflowStateMachine::workflowSequence() const { return workflowSequence_; }

bool WorkflowStateMachine::canEnter(const QString& module) const {
    if (!workflowSequence_.contains(module)) return false;
    if (enterableModules_.isEmpty()) return true;
    return enterableModules_.contains(module);
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
