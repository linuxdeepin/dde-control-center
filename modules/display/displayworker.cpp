#include "displayworker.h"
#include "displaymodel.h"
#include "rotatedialog.h"

#include <QDebug>

const QString DisplayInterface("com.deepin.daemon.Display");

DisplayWorker::DisplayWorker(DisplayModel *model, QObject *parent)
    : QObject(parent),

      m_model(model),

      m_displayInter(DisplayInterface, "/com/deepin/daemon/Display", QDBusConnection::sessionBus(), this)
{
    connect(&m_displayInter, &DisplayInter::MonitorsChanged, this, &DisplayWorker::onMonitorListChanged);

    onMonitorListChanged(m_displayInter.monitors());

    m_displayInter.setSync(false);
}

void DisplayWorker::rotate()
{
    const auto mons = m_model->monitorInterList();

    // ensure only 1 monitor
    Q_ASSERT(mons.size() == 1);

    showRotateDialog(mons.first());
}

void DisplayWorker::onMonitorListChanged(const QList<QDBusObjectPath> &mons)
{
    for (auto op : mons)
    {
        const QString path = op.path();
//        if (m_monitors.contains(path))
//            continue;

        MonitorInter *inter = new MonitorInter(DisplayInterface, path, QDBusConnection::sessionBus(), this);

        m_model->monitorAdded(inter);
    }

    // TODO: remove
}

void DisplayWorker::showRotateDialog(MonitorInter * const inter)
{
    RotateDialog dialog(inter);
    dialog.exec();
}
