#pragma once
#include <QString>
#include <QJsonObject>
#include <QMetaType>

struct StateSample {
    QString channel;
    QJsonObject data;
    qint64 timestampMs{0};
    quint64 sequenceNumber{0};
};

Q_DECLARE_METATYPE(StateSample)
