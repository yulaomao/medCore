#include "CommunicationHub.h"
#include "../../logic/runtime/LogicRuntime.h"
#include <QMetaObject>

CommunicationHub::CommunicationHub(QObject* parent) : QObject(parent) {}

void CommunicationHub::addDataSource(QSharedPointer<SourceBase> source) {
    if (!source) return;
    const QString ch = source->channelName();
    sources_[ch] = source;
    connect(source.data(), &SourceBase::sampleReady,
            this, &CommunicationHub::onSampleReady,
            Qt::QueuedConnection);
}

void CommunicationHub::removeDataSource(const QString& channelName) {
    auto source = sources_.take(channelName);
    if (source) {
        source->stop();
        disconnect(source.data(), nullptr, this, nullptr);
    }
}

void CommunicationHub::setMessageRouter(MessageRouter* router) {
    router_ = router;
}

void CommunicationHub::setLogicRuntime(LogicRuntime* runtime) {
    logicRuntime_ = runtime;
}

void CommunicationHub::start() {
    for (auto& src : sources_) src->start();
}

void CommunicationHub::stop() {
    for (auto& src : sources_) src->stop();
}

void CommunicationHub::onSampleReady(const StateSample& sample) {
    emit stateSampleReceived(sample);

    if (router_) {
        router_->routeMessage(sample.channel, sample.data);
    }

    if (logicRuntime_) {
        QMetaObject::invokeMethod(logicRuntime_, "onStateSample",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, sample.channel),
                                  Q_ARG(QJsonObject, sample.data));
    }
}
