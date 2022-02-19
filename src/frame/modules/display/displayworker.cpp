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
#include "widgets/utils.h"

#include <DApplicationHelper>

#include <QDebug>

using namespace dcc;
using namespace dcc::display;

#define GSETTINGS_MINIMUM_BRIGHTNESS "brightness-minimum"
#define GSETTINGS_SHOW_MUTILSCREEN "show-multiscreen"
#define GSETTINGS_BRIGHTNESS_ENABLE "brightness-enable"

const QString DisplayInterface("com.deepin.daemon.Display");

Q_DECLARE_METATYPE(QList<QDBusObjectPath>)

DisplayWorker::DisplayWorker(DisplayModel *model, QObject *parent, bool isSync)
    : QObject(parent)
    , m_model(model)
    , m_displayInter(DisplayInterface, "/com/deepin/daemon/Display", QDBusConnection::sessionBus(), this)
    , m_dccSettings(new QGSettings("com.deepin.dde.control-center", QByteArray(), this))
    , m_appearanceInter(new AppearanceInter("com.deepin.daemon.Appearance",
                                            "/com/deepin/daemon/Appearance",
                                            QDBusConnection::sessionBus(), this))
    , m_updateScale(false)
    , m_timer(new QTimer(this))
    , m_powerInter(new PowerInter("com.deepin.daemon.Power", "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this))
{
    m_displayInter.setSync(isSync);
    m_appearanceInter->setSync(isSync);
    m_timer->setSingleShot(true);
    m_timer->setInterval(200);

    m_displayDBusInter = new QDBusInterface("com.deepin.daemon.Display",
                                            "/com/deepin/daemon/Display",
                                            "com.deepin.daemon.Display",
                                            QDBusConnection::sessionBus());

    connect(&m_displayInter, &DisplayInter::MonitorsChanged, this, &DisplayWorker::onMonitorListChanged);
    connect(&m_displayInter, &DisplayInter::BrightnessChanged, this, &DisplayWorker::onMonitorsBrightnessChanged);
    connect(&m_displayInter, &DisplayInter::BrightnessChanged, model, &DisplayModel::setBrightnessMap);
    connect(&m_displayInter, &DisplayInter::TouchscreensV2Changed, model, &DisplayModel::setTouchscreenList);
    connect(&m_displayInter, &DisplayInter::TouchMapChanged, model, &DisplayModel::setTouchMap);
    connect(&m_displayInter, &DisplayInter::ScreenHeightChanged, model, &DisplayModel::setScreenHeight);
    connect(&m_displayInter, &DisplayInter::ScreenWidthChanged, model, &DisplayModel::setScreenWidth);
    connect(&m_displayInter, &DisplayInter::DisplayModeChanged, model, &DisplayModel::setDisplayMode);
    connect(&m_displayInter, &DisplayInter::MaxBacklightBrightnessChanged, model, &DisplayModel::setmaxBacklightBrightness);
    connect(&m_displayInter, &DisplayInter::ColorTemperatureModeChanged, model, &DisplayModel::setAdjustCCTmode);
    connect(&m_displayInter, &DisplayInter::ColorTemperatureManualChanged, model, &DisplayModel::setColorTemperature);
    connect(&m_displayInter, static_cast<void (DisplayInter::*)(const QString &) const>(&DisplayInter::PrimaryChanged), model, &DisplayModel::setPrimary);

    //display redSfit/autoLight
    connect(m_powerInter, &PowerInter::HasAmbientLightSensorChanged, m_model, &DisplayModel::autoLightAdjustVaildChanged);
    connect(m_dccSettings, &QGSettings::changed, this, &DisplayWorker::onGSettingsChanged);
    connect(m_timer, &QTimer::timeout, this, [=] {
        m_displayInter.ApplyChanges().waitForFinished();
        m_displayInter.Save().waitForFinished();
    });
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
                               false));

    QDBusPendingCallWatcher *scalewatcher = new QDBusPendingCallWatcher(m_appearanceInter->GetScaleFactor());
    connect(scalewatcher, &QDBusPendingCallWatcher::finished, this, &DisplayWorker::onGetScaleFinished);

    QDBusPendingCallWatcher *screenscaleswatcher = new QDBusPendingCallWatcher(m_appearanceInter->GetScreenScaleFactors());
    connect(screenscaleswatcher, &QDBusPendingCallWatcher::finished, this, &DisplayWorker::onGetScreenScalesFinished);

    onMonitorsBrightnessChanged(m_displayInter.brightness());
    m_model->setBrightnessMap(m_displayInter.brightness());
    onMonitorListChanged(m_displayInter.monitors());

    m_model->setDisplayMode(m_displayInter.displayMode());
    m_model->setTouchscreenList(m_displayInter.touchscreensV2());
    m_model->setTouchMap(m_displayInter.touchMap());
    m_model->setPrimary(m_displayInter.primary());
    m_model->setScreenHeight(m_displayInter.screenHeight());
    m_model->setScreenWidth(m_displayInter.screenWidth());
    m_model->setAdjustCCTmode(m_displayInter.colorTemperatureMode());
    m_model->setColorTemperature(m_displayInter.colorTemperatureManual());
    m_model->setmaxBacklightBrightness(m_displayInter.maxBacklightBrightness());
    m_model->setAutoLightAdjustIsValid(m_powerInter->hasAmbientLightSensor());

    //redshift 依赖X11，当前isXWindowPlatform返回不准确,所以先用环境变量判断
    auto sessionType = qEnvironmentVariable("XDG_SESSION_TYPE");
    const bool isRedshiftValid =
        !sessionType.contains("wayland") && QProcess::execute("which", QStringList() << "redshift") == 0;

    m_model->setRedshiftIsValid(isRedshiftValid);
    m_model->setMinimumBrightnessScale(m_dccSettings->get(GSETTINGS_MINIMUM_BRIGHTNESS).toDouble());
    m_model->setResolutionRefreshEnable(m_dccSettings->get(GSETTINGS_SHOW_MUTILSCREEN).toBool());
    m_model->setBrightnessEnable(m_dccSettings->get(GSETTINGS_BRIGHTNESS_ENABLE).toBool());
}

void DisplayWorker::saveChanges()
{
    m_displayInter.Save().waitForFinished();
    if (m_updateScale)
        setUiScale(m_currentScale);
    m_updateScale = false;
}

void DisplayWorker::switchMode(const int mode, const QString &name)
{
    m_displayInter.SwitchMode(static_cast<uchar>(mode), name).waitForFinished();
}

void DisplayWorker::onMonitorListChanged(const QList<QDBusObjectPath> &mons)
{
    QList<QString> ops;
    for (const auto *mon : m_monitors.keys())
        ops << mon->path();

    qDebug() << mons.size();
    QList<QString> pathList;
    for (const auto &op : mons) {
        const QString path = op.path();
        pathList << path;
        if (!ops.contains(path))
            monitorAdded(path);
    }

    for (const auto &op : ops)
        if (!pathList.contains(op))
            monitorRemoved(op);
}

void DisplayWorker::onMonitorsBrightnessChanged(const BrightnessMap &brightness)
{
    if (brightness.isEmpty())
        return;

    for (auto it = m_monitors.begin(); it != m_monitors.end(); ++it) {
        it.key()->setBrightness(brightness[it.key()->name()]);
    }
}

void DisplayWorker::onGetScaleFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<double> reply = w->reply();

    m_model->setUIScale(reply);

    w->deleteLater();
}

void DisplayWorker::onGetScreenScalesFinished(QDBusPendingCallWatcher *w)
{
    QDBusPendingReply<QMap<QString, double>> reply = w->reply();
    QMap<QString, double> rmap = reply;

    for (auto &m : m_model->monitorList()) {
        if (rmap.find(m->name()) != rmap.end()) {
            m->setScale(rmap.value(m->name()) < 1.0
                            ? m_model->uiScale()
                            : rmap.value(m->name()));
        }
    }

    w->deleteLater();
}

#ifndef DCC_DISABLE_ROTATE
void DisplayWorker::setMonitorRotate(Monitor *mon, const quint16 rotate)
{
    if (m_model->displayMode() == MERGE_MODE) {
        for (auto *m : m_monitors) {
            m->SetRotation(rotate).waitForFinished();
        }
    } else {
        MonitorInter *inter = m_monitors.value(mon);
        inter->SetRotation(rotate).waitForFinished();
    }
}
#endif

void DisplayWorker::setPrimary(const QString &name)
{
    m_displayInter.SetPrimary(name);
}

void DisplayWorker::setMonitorEnable(Monitor *monitor, const bool enable)
{
    MonitorInter *inter = m_monitors.value(monitor);
    inter->Enable(enable).waitForFinished();
    applyChanges();
}

void DisplayWorker::applyChanges()
{
    if (!m_timer->isActive()) {
        m_timer->start();
    }
}

void DisplayWorker::setColorTemperature(int value)
{
    m_displayInter.SetColorTemperature(value).waitForFinished();
}

void DisplayWorker::SetMethodAdjustCCT(int mode)
{
    m_displayInter.SetMethodAdjustCCT(mode);
}

void DisplayWorker::setMonitorResolution(Monitor *mon, const int mode)
{
    MonitorInter *inter = m_monitors.value(mon);
    Q_ASSERT(inter);

    inter->SetMode(static_cast<uint>(mode)).waitForFinished();
}

void DisplayWorker::setMonitorBrightness(Monitor *mon, const double brightness)
{
    m_displayInter.SetAndSaveBrightness(mon->name(), std::max(brightness, m_model->minimumBrightnessScale())).waitForFinished();
}

void DisplayWorker::setMonitorPosition(QHash<Monitor *, QPair<int, int>> monitorPosition)
{
    for (auto it(monitorPosition.cbegin()); it != monitorPosition.cend(); ++it) {
        MonitorInter *inter = m_monitors.value(it.key());
        Q_ASSERT(inter);
        inter->SetPosition(static_cast<short>(it.value().first), static_cast<short>(it.value().second)).waitForFinished();
    }
    applyChanges();
}

void DisplayWorker::setUiScale(const double value)
{
    double rv = value;
    if (rv < 0)
        rv = m_model->uiScale();

    for (auto &mm : m_model->monitorList()) {
        mm->setScale(-1);
    }
    QDBusPendingCall call = m_appearanceInter->SetScaleFactor(rv);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    watcher->waitForFinished();
    if (!watcher->isError()) {
        m_model->setUIScale(rv);
    }
    watcher->deleteLater();
}

void DisplayWorker::setIndividualScaling(Monitor *m, const double scaling)
{
    if (m && scaling >= 1.0) {
        m->setScale(scaling);
    }

    QMap<QString, double> scalemap;
    for (Monitor *m : m_model->monitorList()) {
        scalemap[m->name()] = m_model->monitorScale(m);
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
    });

    process->start("bash", QStringList() << "-c" << QString("systemctl --user %1 redshift.service && systemctl --user %2 redshift.service").arg(serverCmd).arg(cmd));
}

void DisplayWorker::monitorAdded(const QString &path)
{
    MonitorInter *inter = new MonitorInter(DisplayInterface, path, QDBusConnection::sessionBus(), this);
    Monitor *mon = new Monitor(this);

    connect(inter, &MonitorInter::XChanged, mon, &Monitor::setX);
    connect(inter, &MonitorInter::YChanged, mon, &Monitor::setY);
    connect(inter, &MonitorInter::WidthChanged, mon, &Monitor::setW);
    connect(inter, &MonitorInter::HeightChanged, mon, &Monitor::setH);
    connect(inter, &MonitorInter::MmWidthChanged, mon, &Monitor::setMmWidth);
    connect(inter, &MonitorInter::MmHeightChanged, mon, &Monitor::setMmHeight);
    connect(inter, &MonitorInter::RotationChanged, mon, &Monitor::setRotate);
    connect(inter, &MonitorInter::NameChanged, mon, &Monitor::setName);
    connect(inter, &MonitorInter::CurrentModeChanged, mon, &Monitor::setCurrentMode);
    connect(inter, &MonitorInter::BestModeChanged, mon, &Monitor::setBestMode);
    connect(inter, &MonitorInter::CurrentModeChanged, this, [=](Resolution value) {
        if (value.id() == 0) {
            return;
        }
        auto maxWScale = value.width() / 1024.0;
        auto maxHScale = value.height() / 768.0;
        auto maxScale = maxWScale < maxHScale ? maxWScale : maxHScale;
        if ((m_model->uiScale() - maxScale) > 0.01 && maxScale >= 1.0) {
            m_currentScale = 1.0;
            for (int idx = 0; idx * 0.25 + 1.0 <= maxScale; ++idx) {
                m_currentScale = idx * 0.25 + 1.0;
            }
            m_updateScale = true;
        }
    });

    connect(inter, &MonitorInter::ModesChanged, mon, &Monitor::setModeList);
    connect(inter, &MonitorInter::RotationsChanged, mon, &Monitor::setRotateList);
    connect(inter, &MonitorInter::EnabledChanged, mon, &Monitor::setMonitorEnable);
    connect(inter, &MonitorInter::CurrentRotateModeChanged, mon, &Monitor::setCurrentRotateMode);
    connect(&m_displayInter, static_cast<void (DisplayInter::*)(const QString &) const>(&DisplayInter::PrimaryChanged), mon, &Monitor::setPrimary);
    connect(this, &DisplayWorker::requestUpdateModeList, this, [=] {
        mon->setModeList(inter->modes());
    });

    // NOTE: DO NOT using async dbus call. because we need to have a unique name to distinguish each monitor
    Q_ASSERT(inter->isValid());
    mon->setName(inter->name());
    mon->setManufacturer(inter->manufacturer());
    mon->setModel(inter->model());
    QDBusReply<bool> reply = m_displayDBusInter->call("CanSetBrightness", inter->name());
    mon->setCanBrightness(reply.value());
    mon->setMonitorEnable(inter->enabled());
    mon->setCurrentRotateMode(inter->currentRotateMode());
    mon->setPath(path);
    mon->setX(inter->x());
    mon->setY(inter->y());
    mon->setW(inter->width());
    mon->setH(inter->height());
    mon->setRotate(inter->rotation());
    mon->setCurrentMode(inter->currentMode());
    mon->setBestMode(inter->bestMode());
    mon->setModeList(inter->modes());
    if (m_model->isRefreshRateEnable() == false) {
        for (auto resolutionModel : mon->modeList()) {
            if (qFuzzyCompare(resolutionModel.rate(), 0.0) == false) {
                m_model->setRefreshRateEnable(true);
            }
        }
    }
    mon->setRotateList(inter->rotations());
    mon->setPrimary(m_displayInter.primary());
    mon->setMmWidth(inter->mmWidth());
    mon->setMmHeight(inter->mmHeight());

    if (!m_model->brightnessMap().isEmpty()) {
        mon->setBrightness(m_model->brightnessMap()[mon->name()]);
    }

    m_model->monitorAdded(mon);
    m_monitors.insert(mon, inter);

    inter->setSync(false);
}

void DisplayWorker::monitorRemoved(const QString &path)
{
    Monitor *monitor = nullptr;
    for (auto it(m_monitors.cbegin()); it != m_monitors.cend(); ++it) {
        if (it.key()->path() == path) {
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

void DisplayWorker::onGSettingsChanged(const QString &key)
{
    const QVariant &value = m_dccSettings->get(key);

    if (key == GSETTINGS_MINIMUM_BRIGHTNESS || key == "brightnessMinimum") {
        m_model->setMinimumBrightnessScale(value.toDouble());
    } else if (key == GSETTINGS_SHOW_MUTILSCREEN || key == "showMultiscreen") {
        m_model->setResolutionRefreshEnable(value.toBool());
    } else if (key == GSETTINGS_BRIGHTNESS_ENABLE || key == "brightnessEnable") {
        m_model->setBrightnessEnable(value.toBool());
    } else if (key == "resolutionConfig") {
        Q_EMIT requestUpdateModeList();
    }
}

void DisplayWorker::setAmbientLightAdjustBrightness(bool able)
{
    m_powerInter->setAmbientLightAdjustBrightness(able);
}

void DisplayWorker::setTouchScreenAssociation(const QString &monitor, const QString &touchscreenUUID)
{
    m_displayInter.AssociateTouch(monitor, touchscreenUUID);
}

void DisplayWorker::setMonitorResolutionBySize(Monitor *mon, const int width, const int height)
{
    MonitorInter *inter = m_monitors.value(mon);
    Q_ASSERT(inter);

    QDBusPendingCall call = inter->SetModeBySize(static_cast<ushort>(width), static_cast<ushort>(height));
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            qDebug() << call.error().message();
        }
        watcher->deleteLater();
    });
    watcher->waitForFinished();
}
