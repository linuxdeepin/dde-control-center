#ifndef DISPLAYMODEL_H
#define DISPLAYMODEL_H

#include <QObject>
#include <QDBusObjectPath>

#include "displayworker.h"

class DisplayModel : public QObject
{
    Q_OBJECT

    friend class DisplayWorker;

public:
    explicit DisplayModel(QObject *parent = 0);

    const QList<MonitorInter *> monitorInterList() const { return m_monitors; }

signals:
    void monitorListChanged() const;

private:
    void monitorAdded(MonitorInter *inter);

private:
    QList<MonitorInter *> m_monitors;
};

#endif // DISPLAYMODEL_H
