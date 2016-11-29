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
    connect(&m_displayInter, &DisplayInter::ScreenHeightChanged, model, &DisplayModel::setScreenHeight);
    connect(&m_displayInter, &DisplayInter::ScreenWidthChanged, model, &DisplayModel::setScreenWidth);

    onMonitorListChanged(m_displayInter.monitors());
    model->setScreenHeight(m_displayInter.screenHeight());
    model->setScreenWidth(m_displayInter.screenWidth());

    m_displayInter.setSync(false);
}

DisplayWorker::~DisplayWorker()
{
    qDeleteAll(m_monitors.keys());
    qDeleteAll(m_monitors.values());
}

void DisplayWorker::rotate()
{
    const auto mons = m_model->monitorList();

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
        monitorAdded(path);
    }

    // TODO: remove
}

void DisplayWorker::setMonitorRotate(Monitor *mon, const quint16 rotate)
{
    MonitorInter *inter = m_monitors.value(mon);
    Q_ASSERT(inter);

    inter->SetRotation(rotate).waitForFinished();
    m_displayInter.Apply();
}

void DisplayWorker::setMonitorResolution(Monitor *mon, const int mode)
{
    MonitorInter *inter = m_monitors.value(mon);
    Q_ASSERT(inter);

    inter->SetMode(mode).waitForFinished();
    m_displayInter.Apply();
}

void DisplayWorker::loadRotations(Monitor * const mon)
{
    MonitorInter *inter = m_monitors.value(mon);
    Q_ASSERT(inter);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(inter->ListRotations(), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [=] (QDBusPendingCallWatcher *watcher) { loadRotationsFinished(mon, watcher); });
}

void DisplayWorker::loadRotationsFinished(Monitor * const mon, QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<RotationList> reply = *watcher;
    mon->setRotateList(reply.value());
    watcher->deleteLater();
}

void DisplayWorker::loadModes(Monitor * const mon)
{
    MonitorInter *inter = m_monitors.value(mon);
    Q_ASSERT(inter);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(inter->ListModes(), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [=] (QDBusPendingCallWatcher *watcher) { loadModesFinished(mon, watcher); });
}

void DisplayWorker::loadModesFinished(Monitor * const mon, QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<ResolutionList> reply = *watcher;
    mon->setModeList(reply.value());
    watcher->deleteLater();
}

void DisplayWorker::monitorAdded(const QString &path)
{
    MonitorInter *inter = new MonitorInter(DisplayInterface, path, QDBusConnection::sessionBus(), this);
    Monitor *mon = new Monitor(this);

    connect(inter, &MonitorInter::XChanged, mon, &Monitor::setX);
    connect(inter, &MonitorInter::YChanged, mon, &Monitor::setY);
    connect(inter, &MonitorInter::WidthChanged, mon, &Monitor::setW);
    connect(inter, &MonitorInter::HeightChanged, mon, &Monitor::setH);
    connect(inter, &MonitorInter::RotationChanged, mon, &Monitor::setRotate);

    inter->setSync(false);

    mon->setX(inter->x());
    mon->setY(inter->y());
    mon->setW(inter->width());
    mon->setH(inter->height());
    mon->setRotate(inter->rotation());

    m_model->monitorAdded(mon);
    m_monitors.insert(mon, inter);

    // TODO: optimize
    loadRotations(mon);
    loadModes(mon);
}

void DisplayWorker::showRotateDialog(Monitor * const mon)
{
    RotateDialog dialog(mon);

    connect(&dialog, &RotateDialog::requestRotate, this, &DisplayWorker::setMonitorRotate);

    dialog.exec();
}
