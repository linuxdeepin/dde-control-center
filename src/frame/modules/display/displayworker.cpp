/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "displayworker.h"
#include "displaymodel.h"
#include "monitorsettingdialog.h"
#include "widgets/utils.h"

#include <QDebug>

using namespace dcc;
using namespace dcc::display;

#define GSETTINGS_MINIMUM_BRIGHTNESS    "brightness-minimum"

const QString DisplayInterface("com.deepin.daemon.Display");

DisplayWorker::DisplayWorker(DisplayModel *model, QObject *parent)
    : QObject(parent),

      m_model(model),
      m_displayInter(DisplayInterface, "/com/deepin/daemon/Display", QDBusConnection::sessionBus(), this),
      m_dccSettings(new QGSettings("com.deepin.dde.control-center", QByteArray(), this)),
      m_appearanceInter(new AppearanceInter("com.deepin.daemon.Appearance",
                                      "/com/deepin/daemon/Appearance",
                                      QDBusConnection::sessionBus(), this))
{
    // TODO:
    model->setPrimary(m_displayInter.primary());

    m_displayInter.setSync(false);
    m_appearanceInter->setSync(false);

    connect(&m_displayInter, &DisplayInter::MonitorsChanged, this, &DisplayWorker::onMonitorListChanged);
    connect(&m_displayInter, &DisplayInter::BrightnessChanged, this, &DisplayWorker::onMonitorsBrightnessChanged);
    connect(&m_displayInter, &DisplayInter::BrightnessChanged, model, &DisplayModel::setBrightnessMap);
    connect(&m_displayInter, &DisplayInter::ScreenHeightChanged, model, &DisplayModel::setScreenHeight);
    connect(&m_displayInter, &DisplayInter::ScreenWidthChanged, model, &DisplayModel::setScreenWidth);
    connect(&m_displayInter, &DisplayInter::DisplayModeChanged, model, &DisplayModel::setDisplayMode);
    connect(&m_displayInter, &DisplayInter::CurrentCustomIdChanged, model, &DisplayModel::setCurrentConfig);
    connect(&m_displayInter, &DisplayInter::CustomIdListChanged, model, &DisplayModel::setConfigList);
//    connect(&m_displayInter, &DisplayInter::HasCustomConfigChanged, model, &DisplayModel::setHasConfig);
    connect(&m_displayInter, static_cast<void (DisplayInter::*)(const QString &) const>(&DisplayInter::PrimaryChanged), model, &DisplayModel::setPrimary);

    onMonitorListChanged(m_displayInter.monitors());
    onMonitorsBrightnessChanged(m_displayInter.brightness());
    model->setScreenHeight(m_displayInter.screenHeight());
    model->setScreenWidth(m_displayInter.screenWidth());
    model->setConfigList(m_displayInter.customIdList());
    model->setCurrentConfig(m_displayInter.currentCustomId());
//    model->setHasConfig(m_displayInter.hasCustomConfig());
    model->setDisplayMode(m_displayInter.displayMode());

    const bool isRedshiftValid = QProcess::execute("which", QStringList() << "redshift") == 0;

    if (isRedshiftValid)
        updateNightModeStatus();

    m_model->setRedshiftIsValid(isRedshiftValid);
    m_model->setMinimumBrightnessScale(m_dccSettings->get(GSETTINGS_MINIMUM_BRIGHTNESS).toDouble());
}

DisplayWorker::~DisplayWorker()
{
    qDeleteAll(m_monitors.keys());
    qDeleteAll(m_monitors.values());
}

void DisplayWorker::active()
{
    m_model->setAllowEnableMultiScaleRatio(
        valueByQSettings<bool>(DCC_CONFIG_FILES,
                               "Display",
                               "AllowEnableMultiScaleRatio",
                               false)
    );

    QDBusPendingCallWatcher *scalewatcher = new QDBusPendingCallWatcher(m_appearanceInter->GetScaleFactor());
    connect(scalewatcher, &QDBusPendingCallWatcher::finished, this, &DisplayWorker::onGetScaleFinished);

    QDBusPendingCallWatcher *screenscaleswatcher = new QDBusPendingCallWatcher(m_appearanceInter->GetScreenScaleFactors());
    connect(screenscaleswatcher, &QDBusPendingCallWatcher::finished, this, &DisplayWorker::onGetScreenScalesFinished);
}

void DisplayWorker::saveChanges()
{
    qDebug() << Q_FUNC_INFO;

    m_displayInter.Save().waitForFinished();
}

void DisplayWorker::discardChanges()
{
    qDebug() << Q_FUNC_INFO;

    m_displayInter.ResetChanges().waitForFinished();
}

void DisplayWorker::mergeScreens()
{
    qDebug() << Q_FUNC_INFO;

    m_model->setIsMerge(true);

    // TODO: make asynchronous
    const QList<Resolution> commonModes = m_displayInter.ListOutputsCommonModes();
    Q_ASSERT(!commonModes.isEmpty());

    const auto mode = commonModes.first();
    const auto rotate = m_model->primaryMonitor()->rotate();
    const auto brightness = m_model->primaryMonitor()->brightness();

    QList<QDBusPendingReply<>> replys;

    for (auto *mon : m_model->monitorList())
    {
        auto *mInter = m_monitors[mon];
        Q_ASSERT(mInter);

        replys << mInter->SetPosition(0, 0);
        replys << mInter->SetModeBySize(mode.width(), mode.height());
        replys << mInter->SetRotation(rotate);
        replys << m_displayInter.SetBrightness(mon->name(), brightness);
    }

    for (auto r : replys)
        r.waitForFinished();

    m_displayInter.ApplyChanges().waitForFinished();
}

void DisplayWorker::splitScreens()
{
    qDebug() << Q_FUNC_INFO;

    m_model->setIsMerge(false);

    const auto mList = m_model->monitorList();
    Q_ASSERT(mList.size() == 2);

    auto *primary = m_model->primaryMonitor();
    Q_ASSERT(m_monitors.contains(primary));
    m_monitors[primary]->SetPosition(0, 0).waitForFinished();

    int xOffset = primary->w();
    for (auto *mon : mList)
    {
        // pass primary
        if (mon == primary)
            continue;

        Q_ASSERT(m_monitors.contains(mon));
        auto *mInter = m_monitors[mon];

        mInter->SetPosition(xOffset, 0).waitForFinished();
        xOffset += mon->w();
    }

    m_displayInter.ApplyChanges();
}

void DisplayWorker::duplicateMode()
{
    switchMode(MERGE_MODE);
    saveChanges();
}

void DisplayWorker::extendMode()
{
    switchMode(EXTEND_MODE);
    saveChanges();
}

void DisplayWorker::onlyMonitor(const QString &monName)
{
    switchMode(SINGLE_MODE, monName);
}

void DisplayWorker::createConfig(const QString &config)
{
    const auto reply = m_displayInter.SwitchMode(CUSTOM_MODE, config);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply);
    watcher->setProperty("ConfigName", config);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, &DisplayWorker::onCreateConfigFinshed);
}

void DisplayWorker::switchConfig(const QString &config)
{
    switchMode(CUSTOM_MODE, config);
}

void DisplayWorker::deleteConfig(const QString &config)
{
    m_displayInter.DeleteCustomMode(config);
}

void DisplayWorker::modifyConfigName(const QString &oldName, const QString &newName)
{
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_displayInter.ModifyConfigName(oldName, newName));

    connect(w, &QDBusPendingCallWatcher::finished, this, &DisplayWorker::onModifyConfigNameFinished);
}

void DisplayWorker::switchMode(const int mode, const QString &name)
{
    qDebug() << Q_FUNC_INFO << mode << name;

    m_displayInter.SwitchMode(mode, name).waitForFinished();
}

void DisplayWorker::onMonitorListChanged(const QList<QDBusObjectPath> &mons)
{
    QList<QString> ops;
    for (const auto *mon : m_monitors.keys())
        ops << mon->path();

    QList<QString> pathList;
    for (const auto op : mons)
    {
        const QString path = op.path();
        pathList << path;
        if (!ops.contains(path))
            monitorAdded(path);
    }

    for (const auto op : ops)
        if (!pathList.contains(op))
            monitorRemoved(op);
}

void DisplayWorker::onMonitorsBrightnessChanged(const BrightnessMap &brightness)
{
    if (brightness.isEmpty()) return;

    for (auto it = m_monitors.begin(); it != m_monitors.end(); ++it) {
        it.key()->setBrightness(brightness[it.key()->name()]);
    }
}

void DisplayWorker::onModifyConfigNameFinished(QDBusPendingCallWatcher *w)
{
    w->deleteLater();
}

void DisplayWorker::onGetScaleFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<double> reply = w->reply();

    m_model->setUIScale(reply);

    w->deleteLater();
}

void DisplayWorker::onGetScreenScalesFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QMap<QString,double>> reply = w->reply();
    QMap<QString,double> rmap = reply;

    for (auto& m : m_model->monitorList()){
        if (rmap.find(m->name()) != rmap.end()){
            m->setScale(rmap[m->name()]);
        }
    }

    w->deleteLater();
}

void DisplayWorker::onCreateConfigFinshed(QDBusPendingCallWatcher *w)
{
    const QString &name = w->property("ConfigName").toString();

    Q_EMIT m_model->configCreated(name);

    w->deleteLater();
}

#ifndef DCC_DISABLE_ROTATE
void DisplayWorker::setMonitorRotate(Monitor *mon, const quint16 rotate)
{
    MonitorInter *inter = m_monitors.value(mon);
    Q_ASSERT(inter);

    inter->SetRotation(rotate).waitForFinished();
    m_displayInter.ApplyChanges();
}

void DisplayWorker::setMonitorRotateAll(const quint16 rotate)
{
    for (auto *mi : m_monitors)
        mi->SetRotation(rotate).waitForFinished();

    m_displayInter.ApplyChanges();
}
#endif

void DisplayWorker::setPrimary(const int index)
{
    m_displayInter.SetPrimary(m_model->monitorList()[index]->name());
}

void DisplayWorker::setMonitorEnable(Monitor *mon, const bool enabled)
{
    MonitorInter *inter = m_monitors.value(mon);
    Q_ASSERT(inter);

    inter->Enable(enabled).waitForFinished();
    m_displayInter.ApplyChanges();
}

void DisplayWorker::applyChanges()
{
    m_displayInter.ApplyChanges().waitForFinished();
}

void DisplayWorker::setMonitorResolution(Monitor *mon, const int mode)
{
    MonitorInter *inter = m_monitors.value(mon);
    Q_ASSERT(inter);

    inter->SetMode(mode).waitForFinished();
    m_displayInter.ApplyChanges().waitForFinished();
}

void DisplayWorker::setMonitorBrightness(Monitor *mon, const double brightness)
{
    m_displayInter.SetAndSaveBrightness(mon->name(), std::max(brightness, m_model->minimumBrightnessScale())).waitForFinished();
}

void DisplayWorker::setMonitorPosition(Monitor *mon, const int x, const int y)
{
    MonitorInter *inter = m_monitors.value(mon);
    Q_ASSERT(inter);

    inter->SetPosition(x, y).waitForFinished();
    m_displayInter.ApplyChanges().waitForFinished();
}

void DisplayWorker::setUiScale(const double value)
{
    double rv=value;
    if (rv < 0) rv = m_model->uiScale();

    for (auto &mm : m_model->monitorList()) {
        mm->setScale(-1);
    }
    QDBusPendingCall call = m_appearanceInter->SetScaleFactor(rv);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            Q_EMIT m_model->uiScaleChanged(rv);
            qWarning() << call.error();
        }
        watcher->deleteLater();
    });
}

void DisplayWorker::setIndividualScaling(Monitor *m, const double scaling)
{
    if (m && scaling > 0) {
        m->setScale(scaling);
    }

    double primaryscale = m_model->primaryMonitor()->scale();
    m_appearanceInter->SetScaleFactor(primaryscale);

    QMap<QString, double> scalemap;
    for (auto& m : m_model->monitorList()){
        if (m->scale() > 0) {
            scalemap[m->name()]=m->scale();
        }
        else {
            scalemap[m->name()]=1;
        }
    }
    m_appearanceInter->SetScreenScaleFactors(scalemap);
}

void DisplayWorker::setNightMode(const bool nightmode)
{
    QProcess *process = new QProcess(this);

    QString cmd;
    QString serverCmd;
    if (nightmode) {
        cmd = "start";
        serverCmd = "enable";
    } else {
        cmd = "stop";
        serverCmd = "disable";
    }

    connect(process, static_cast<void (QProcess::*)(int exitCode)>(&QProcess::finished), this, [=] {
        process->close();
        process->deleteLater();
        // reload
        updateNightModeStatus();
    });

    process->start("bash", QStringList() << "-c" << QString("systemctl --user %1 redshift.service && systemctl --user %2 redshift.service")
                  .arg(serverCmd)
                  .arg(cmd));

    m_model->setRedshiftSetting(true);
}

//void DisplayWorker::loadRotations(Monitor * const mon)
//{
//    MonitorInter *inter = m_monitors.value(mon);
//    Q_ASSERT(inter);

//    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(inter->ListRotations(), this);
//    connect(watcher, &QDBusPendingCallWatcher::finished, [=] (QDBusPendingCallWatcher *watcher) { loadRotationsFinished(mon, watcher); });
//}

//void DisplayWorker::loadRotationsFinished(Monitor * const mon, QDBusPendingCallWatcher *watcher)
//{
//    QDBusPendingReply<RotationList> reply = *watcher;
//    mon->setRotateList(reply.value());
//    watcher->deleteLater();
//}

//void DisplayWorker::loadModes(Monitor * const mon)
//{
//    MonitorInter *inter = m_monitors.value(mon);
//    Q_ASSERT(inter);

//    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(inter->ListModes(), this);
//    connect(watcher, &QDBusPendingCallWatcher::finished, [=] (QDBusPendingCallWatcher *watcher) { loadModesFinished(mon, watcher); });
//}

//void DisplayWorker::loadModesFinished(Monitor * const mon, QDBusPendingCallWatcher *watcher)
//{
//    QDBusPendingReply<ResolutionList> reply = *watcher;
//    mon->setModeList(reply.value());
//    watcher->deleteLater();
//}

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
    connect(inter, &MonitorInter::ModesChanged, mon, &Monitor::setModeList);
    connect(inter, &MonitorInter::RotationsChanged, mon, &Monitor::setRotateList);
    connect(&m_displayInter, static_cast<void (DisplayInter::*)(const QString &) const>(&DisplayInter::PrimaryChanged), mon, &Monitor::setPrimary);

    // NOTE: DO NOT using async dbus call. because we need to have a unique name to distinguish each monitor
    Q_ASSERT(inter->isValid());
    mon->setName(inter->name());

    inter->setSync(false);

    mon->setPath(path);
    mon->setX(inter->x());
    mon->setY(inter->y());
    mon->setW(inter->width());
    mon->setH(inter->height());
    mon->setRotate(inter->rotation());
    mon->setCurrentMode(inter->currentMode());
    mon->setModeList(inter->modes());
    mon->setRotateList(inter->rotations());
    mon->setPrimary(m_displayInter.primary());

    if (!m_model->brightnessMap().isEmpty()) {
        mon->setBrightness(m_model->brightnessMap()[mon->name()]);
    }

    m_model->monitorAdded(mon);
    m_monitors.insert(mon, inter);
}

void DisplayWorker::monitorRemoved(const QString &path)
{
    Monitor *monitor = nullptr;
    for (auto it(m_monitors.cbegin()); it != m_monitors.cend(); ++it)
    {
        if (it.key()->path() == path)
        {
            monitor = it.key();
            break;
        }
    }
    if (!monitor)
        return;

    m_model->monitorRemoved(monitor);

    m_monitors[monitor]->deleteLater();
    m_monitors.remove(monitor);

    monitor->deleteLater();
}

void DisplayWorker::updateNightModeStatus()
{
    QProcess *process = new QProcess;

    connect(process, &QProcess::readyRead, this, [=] {
        m_model->setIsNightMode(process->readAll().replace("\n","") == "active");
        m_model->setRedshiftSetting(false);
        process->close();
        process->deleteLater();
    });

    process->start("systemctl", QStringList() << "--user" << "is-active" << "redshift.service");
}

void DisplayWorker::onGSettingsChanged(const QString &key)
{
    const QVariant &value = m_dccSettings->get(key);

    if (key == GSETTINGS_MINIMUM_BRIGHTNESS)
        m_model->setMinimumBrightnessScale(value.toDouble());
}

void DisplayWorker::record() {
    const int displayMode { m_model->displayMode() };
    const QString config { displayMode == CUSTOM_MODE ? m_model->config() : m_model->primary() };

    m_model->setLastConfig(std::pair<int, QString>(displayMode, config));
}

void DisplayWorker::restore() {
    const std::pair<int, QString> lastConfig { m_model->lastConfig() };

    switch (lastConfig.first)
    {
        case CUSTOM_MODE: {
            discardChanges();
            switchMode(lastConfig.first, lastConfig.second);
            saveChanges();
            break;
        }
        case MERGE_MODE:
            mergeScreens();
            break;
        case EXTEND_MODE:
            extendMode();
            break;
        case SINGLE_MODE:
            onlyMonitor(lastConfig.second);
            break;
        default:
            break;
    }
}
