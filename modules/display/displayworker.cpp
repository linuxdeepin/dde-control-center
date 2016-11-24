#include "displayworker.h"

#include <QDebug>

const QString DisplayInterface("com.deepin.daemon.Display");

DisplayWorker::DisplayWorker(QObject *parent)
    : QObject(parent),

      m_displayInter(DisplayInterface, "/com/deepin/daemon/Display", QDBusConnection::sessionBus(), this)
{
    for (auto path : m_displayInter.monitors())
    {
        qDebug() << path.path();

        MonitorInter *inter = new MonitorInter(DisplayInterface, path.path(), QDBusConnection::sessionBus(), this);
//        qDebug() << inter.x();
        Resolution r = inter->bestMode();
        qDebug() << r.id() << inter->lastError();
    }

    m_displayInter.setSync(false);
}
