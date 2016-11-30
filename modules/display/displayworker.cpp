#include "displayworker.h"
#include "displaymodel.h"
#include "rotatedialog.h"
#include "monitorsettingdialog.h"

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
    connect(&m_displayInter, static_cast<void (DisplayInter::*)(const QString &) const>(&DisplayInter::PrimaryChanged), model, &DisplayModel::setPrimary);
    connect(&m_displayInter, &DisplayInter::BrightnessChanged, this, &DisplayWorker::onMonitorsBrightnessChanged);

    onMonitorListChanged(m_displayInter.monitors());
    onMonitorsBrightnessChanged(m_displayInter.brightness());
    model->setScreenHeight(m_displayInter.screenHeight());
    model->setScreenWidth(m_displayInter.screenWidth());
    model->setPrimary(m_displayInter.primary());

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

void DisplayWorker::showCustomSettings()
{
    MonitorSettingDialog dialog(m_model);

    connect(&dialog, &MonitorSettingDialog::requestSetMonitorMode, this, &DisplayWorker::setMonitorResolution);
    connect(&dialog, &MonitorSettingDialog::requestSetMonitorBrightness, this, &DisplayWorker::setMonitorBrightness);

    // discard or save
    if (dialog.exec() == QDialog::Accepted)
        m_displayInter.SaveChanges();
    else
        m_displayInter.Reset();

//    MonitorSettingDialog *primryDialog = nullptr;
//    QList<MonitorSettingDialog *> dialogs;

//    for (auto mon : m_monitors.keys())
//    {
//        MonitorSettingDialog *dialog = new MonitorSettingDialog(m_model, mon);
//        if (!primryDialog && m_model->primary() == mon->name())
//        {
//            primryDialog = dialog;
//            dialog->setPrimary();
//        }

//        dialog->show();
//        dialogs.append(dialog);
//    }
//    Q_ASSERT(primryDialog);

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

void DisplayWorker::onMonitorsBrightnessChanged(const BrightnessMap &brightness)
{
    for (auto it(brightness.cbegin()); it != brightness.cend(); ++it)
        updateMonitorBrightness(it.key(), it.value());
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

void DisplayWorker::setMonitorBrightness(Monitor *mon, const double brightness)
{
    m_displayInter.SetBrightness(mon->name(), brightness).waitForFinished();
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
    connect(inter, &MonitorInter::NameChanged, mon, &Monitor::setName);
    connect(inter, &MonitorInter::CurrentModeChanged, mon, &Monitor::setCurrentMode);

    inter->setSync(false);

    mon->setX(inter->x());
    mon->setY(inter->y());
    mon->setW(inter->width());
    mon->setH(inter->height());
    mon->setRotate(inter->rotation());
    mon->setName(inter->name());
    mon->setCurrentMode(inter->currentMode());

    m_model->monitorAdded(mon);
    m_monitors.insert(mon, inter);

    // TODO: optimize
    loadRotations(mon);
    loadModes(mon);
}

void DisplayWorker::updateMonitorBrightness(const QString &monName, const double brightness)
{
    for (auto mon : m_monitors.keys())
    {
        if (mon->name() == monName)
        {
            mon->setBrightness(brightness);
            return;
        }
    }
}

void DisplayWorker::showRotateDialog(Monitor * const mon)
{
    RotateDialog dialog(mon);

    connect(&dialog, &RotateDialog::requestRotate, this, &DisplayWorker::setMonitorRotate);

    dialog.exec();
}
