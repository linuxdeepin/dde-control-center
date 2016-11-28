#ifndef DISPLAYMODEL_H
#define DISPLAYMODEL_H

#include <QObject>
#include <QDBusObjectPath>

#include "displayworker.h"
#include "monitor.h"

class DisplayModel : public QObject
{
    Q_OBJECT

    friend class DisplayWorker;

public:
    explicit DisplayModel(QObject *parent = 0);

    const QList<Monitor *> monitorList() const { return m_monitors; }

signals:
    void monitorListChanged() const;

private:
    void monitorAdded(Monitor *mon);

private:
    QList<Monitor *> m_monitors;
};

#endif // DISPLAYMODEL_H
