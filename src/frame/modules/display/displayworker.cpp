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

#include <DApplicationHelper>

#include <QDebug>

using namespace dcc;
using namespace dcc::display;

#define GSETTINGS_MINIMUM_BRIGHTNESS    "brightness-minimum"

const QString DisplayInterface("com.deepin.daemon.Display");

Q_DECLARE_METATYPE(QList<QDBusObjectPath>)

DisplayWorker::DisplayWorker(DisplayModel *model, QObject *parent, bool isSync)
    : QObject(parent)
    , m_model(model)
{
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

    onMonitorsBrightnessChanged(m_displayInter->brightness());
    onMonitorListChanged(m_displayInter->monitors());

    m_model->setDisplayMode(m_displayInter->displayMode());
    m_model->setTouchscreenList(m_displayInter->touchscreens());
    m_model->setTouchMap(m_displayInter->touchMap());
    m_model->setPrimary(m_displayInter->primary());
    m_model->setScreenHeight(m_displayInter->screenHeight());
    m_model->setScreenWidth(m_displayInter->screenWidth());
    m_model->setAdjustCCTmode(m_displayInter->colorTemperatureMode());
    m_model->setColorTemperature(m_displayInter->colorTemperatureManual());
    m_model->setmaxBacklightBrightness(m_displayInter->maxBacklightBrightness());
    m_model->setConfigList(m_displayInter->customIdList());
    m_model->setCurrentConfig(m_displayInter->currentCustomId());

    m_model->setAutoLightAdjustIsValid(m_powerInter->hasAmbientLightSensor());

    m_model->setMouseLeftHand(m_mouseInter->leftHanded());

    //redshift 依赖X11，当前isXWindowPlatform返回不准确,所以先用环境变量判断
    //   DGuiApplicationHelper::isXWindowPlatform() const bool isRedshiftValid = DGuiApplicationHelper::isXWindowPlatform() && QProcess::execute("which", QStringList() << "redshift") == 0;
    auto sessionType = qEnvironmentVariable("XDG_SESSION_TYPE");
    const bool isRedshiftValid =
        !sessionType.contains("wayland") &&
        QProcess::execute("which", QStringList() << "redshift") == 0;

    m_model->setRedshiftIsValid(isRedshiftValid);
    m_model->setMinimumBrightnessScale(
                m_dccSettings->get(GSETTINGS_MINIMUM_BRIGHTNESS).toDouble());
}

void DisplayWorker::init()
{
    m_displayInter = new DisplayInter(DisplayInterface, "/com/deepin/daemon/Display", QDBusConnection::sessionBus(), this);
    m_dccSettings = new QGSettings("com.deepin.dde.control-center", QByteArray(), this);
    m_appearanceInter = new AppearanceInter("com.deepin.daemon.Appearance",
                                          "/com/deepin/daemon/Appearance",
                                          QDBusConnection::sessionBus(), this);
    m_powerInter = new PowerInter("com.deepin.daemon.Power", "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this);
    m_mouseInter = new MouseInter("com.deepin.daemon.InputDevices", "/com/deepin/daemon/InputDevice/Mouse", QDBusConnection::sessionBus(), this);

    m_displayInter->setSync(true);
    m_appearanceInter->setSync(true);

    m_displayDBusInter = new QDBusInterface("com.deepin.daemon.Display",
                               "/com/deepin/daemon/Display",
                               "com.deepin.daemon.Display",
                               QDBusConnection::sessionBus());

    connect(m_displayInter, &DisplayInter::MonitorsChanged, this, &DisplayWorker::onMonitorListChanged);
    connect(m_displayInter, &DisplayInter::BrightnessChanged, this, &DisplayWorker::onMonitorsBrightnessChanged);
    connect(m_displayInter, &DisplayInter::BrightnessChanged, m_model, &DisplayModel::setBrightnessMap);
    connect(m_displayInter, &DisplayInter::TouchscreensChanged, m_model, &DisplayModel::setTouchscreenList);
    connect(m_displayInter, &DisplayInter::TouchMapChanged, m_model, &DisplayModel::setTouchMap);
    connect(m_displayInter, &DisplayInter::ScreenHeightChanged, m_model, &DisplayModel::setScreenHeight);
    connect(m_displayInter, &DisplayInter::ScreenWidthChanged, m_model, &DisplayModel::setScreenWidth);
    connect(m_displayInter, &DisplayInter::DisplayModeChanged, m_model, &DisplayModel::setDisplayMode);
    connect(m_displayInter, &DisplayInter::CurrentCustomIdChanged, m_model, &DisplayModel::setCurrentConfig);
    connect(m_displayInter, &DisplayInter::CustomIdListChanged, m_model, &DisplayModel::setConfigList);
    connect(m_displayInter, &DisplayInter::MaxBacklightBrightnessChanged, m_model, &DisplayModel::setmaxBacklightBrightness);
    connect(m_displayInter, &DisplayInter::ColorTemperatureModeChanged, m_model, &DisplayModel::setAdjustCCTmode);
    connect(m_displayInter, &DisplayInter::ColorTemperatureManualChanged, m_model, &DisplayModel::setColorTemperature);
    connect(m_displayInter, static_cast<void (DisplayInter::*)(const QString &) const>(&DisplayInter::PrimaryChanged), m_model, &DisplayModel::setPrimary);

    //display redSfit/autoLight
    connect(m_powerInter, &PowerInter::HasAmbientLightSensorChanged, m_model, &DisplayModel::autoLightAdjustVaildChanged);
    connect(m_mouseInter, &MouseInter::LeftHandedChanged, m_model, &DisplayModel::setMouseLeftHand);
}

void DisplayWorker::saveChanges()
{
    qDebug() << Q_FUNC_INFO;

    m_displayInter->Save().waitForFinished();
}

void DisplayWorker::discardChanges()
{
    qDebug() << Q_FUNC_INFO;

    m_displayInter->ResetChanges().waitForFinished();
}

void DisplayWorker::mergeScreens()
{
    qDebug() << Q_FUNC_INFO;

    m_model->setIsMerge(true);

    m_model->monitorList()[0]->setLastPoint(m_model->monitorList()[0]->x(), m_model->monitorList()[0]->y());
    m_model->monitorList()[1]->setLastPoint(m_model->monitorList()[1]->x(), m_model->monitorList()[1]->y());

    // TODO: make asynchronous
    auto monis = m_monitors.keys();
    auto firstMoni = monis.first();
    auto modes = firstMoni->modeList();

    int maxSize = 0;
    Resolution bestMode = modes.first();
    for (auto m : modes) {
        bool isCommon = true;
        for (int i = 1; i < monis.size(); ++i) {
            if (!monis[i]->hasResolution(m)) {
                isCommon = false;
                break;
            }
        }

        if (!isCommon) {
            continue;
        }

        qDebug() << "get same resolution:" << m.width() << " x " << m.height();
        auto ts = m.width() * m.height();
        if (ts <= maxSize)
            continue;

        bestMode = m;
        maxSize = ts;
    }

    qDebug() << "get best Resolution :" << bestMode.width() << " x " << bestMode.height();
    const auto mode = bestMode;
    auto rotate = m_model->primaryMonitor()->rotate();
    const auto brightness = m_model->primaryMonitor()->brightness();
    for (auto *mon : m_model->monitorList()) {
        if (mon->rotate() != rotate) {
            rotate = 1;
        }
    }
    QList<QDBusPendingReply<>> replys;

    for (auto *mon : m_model->monitorList()) {
        auto *mInter = m_monitors[mon];
        Q_ASSERT(mInter);

        replys << mInter->SetPosition(0, 0);
        replys << mInter->SetModeBySize(static_cast<ushort>(mode.width()), static_cast<ushort>(mode.height()));
        replys << mInter->SetRotation(rotate);
        replys << m_displayInter->SetBrightness(mon->name(), brightness);
    }

    for (auto r : replys)
        r.waitForFinished();

    m_displayInter->ApplyChanges().waitForFinished();
}

void DisplayWorker::splitScreens()
{
    qDebug() << Q_FUNC_INFO;

    m_model->setIsMerge(false);

    const auto mList = m_model->monitorList();
    Q_ASSERT(mList.size() == 2);

    auto *primary = m_model->primaryMonitor();
    Q_ASSERT(m_monitors.contains(primary));
    m_monitors[primary]->SetPosition(static_cast<short>(m_model->primaryMonitor()->getLastPoint().x()), static_cast<short>(m_model->primaryMonitor()->getLastPoint().y())).waitForFinished();
    int xOffset = primary->bestMode().width();

    for (auto *mon : mList) {
        // pass primary
        Q_ASSERT(m_monitors.contains(mon));
        auto *mInter = m_monitors[mon];
        // 设置最好模式
        mInter->SetMode(static_cast<uint>(mon->bestMode().id())).waitForFinished();
        mInter->SetRotation(1).waitForFinished();

        if (mon == primary)
            continue;

        if (mon->getLastPoint() == m_model->primaryMonitor()->getLastPoint()) {
            mInter->SetPosition(static_cast<short>(xOffset), 0).waitForFinished();
        } else {
            mInter->SetPosition(static_cast<short>(mon->getLastPoint().x()), static_cast<short>(mon->getLastPoint().y())).waitForFinished();
        }
        xOffset += mon->bestMode().width();
    }

    m_displayInter->ApplyChanges();
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
    const auto reply = m_displayInter->SwitchMode(CUSTOM_MODE, config);
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
    m_displayInter->DeleteCustomMode(config);
}

void DisplayWorker::modifyConfigName(const QString &oldName, const QString &newName)
{
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_displayInter->ModifyConfigName(oldName, newName));

    connect(w, &QDBusPendingCallWatcher::finished, this, &DisplayWorker::onModifyConfigNameFinished);
}

void DisplayWorker::switchMode(const int mode, const QString &name)
{
    qDebug() << Q_FUNC_INFO << mode << name;

    m_displayInter->SwitchMode(static_cast<uchar>(mode), name).waitForFinished();
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
    QDBusPendingReply<QMap<QString, double>> reply = w->reply();
    QMap<QString, double> rmap = reply;

    for (auto &m : m_model->monitorList()) {
        if (rmap.find(m->name()) != rmap.end()) {
            m->setScale(rmap.value(m->name()) < 1.0
                        ? m_model->uiScale() : rmap.value(m->name()));
        }
    }

    w->deleteLater();
}

void DisplayWorker::onCreateConfigFinshed(QDBusPendingCallWatcher *w)
{
    const QString &name = w->property("ConfigName").toString();

    Q_EMIT m_model->configCreated(name);
    m_model->setDisplayMode(0);

    w->deleteLater();
}

#ifndef DCC_DISABLE_ROTATE
void DisplayWorker::setMonitorRotate(Monitor *mon, const quint16 rotate)
{
    MonitorInter *inter = m_monitors.value(mon);
    Q_ASSERT(inter);

    inter->SetRotation(rotate).waitForFinished();
    for (auto tm = m_monitors.begin(); tm != m_monitors.end(); ++tm) {
        if (m_model->isMerge()) {
            tm.value()->SetPosition(0, 0);
        } else {
            tm.value()->SetPosition(static_cast<short>(tm.key()->x()), static_cast<short>(tm.key()->y()));
        }
    }
    m_displayInter->ApplyChanges();
}

void DisplayWorker::setMonitorRotateAll(const quint16 rotate)
{
    qDebug() << rotate;
    for (auto *mi : m_monitors)
        mi->SetRotation(rotate).waitForFinished();

    qDebug() << m_displayInter->ApplyChanges().error();
}
#endif

void DisplayWorker::setPrimary(const int index)
{
    m_displayInter->SetPrimary(m_model->monitorList()[index]->name());
}

void DisplayWorker::setPrimaryByName(const QString &name)
{
    m_displayInter->SetPrimary(name);
}

void DisplayWorker::setMonitorEnable(Monitor *mon, const bool enabled)
{
    MonitorInter *inter = m_monitors.value(mon);
    Q_ASSERT(inter);

    inter->Enable(enabled).waitForFinished();
    m_displayInter->ApplyChanges();
}

void DisplayWorker::applyChanges()
{
    m_displayInter->ApplyChanges().waitForFinished();
}

void DisplayWorker::onMonitorEnable(Monitor *monitor, const bool enabled)
{
    //如果是灭屏幕，且当前亮的屏幕只有一个，直接返回
    if (!enabled) {
        int enableCount = 0;
        for (auto *tm : m_model->monitorList()) {
            // pass primary
            enableCount += tm->enable() ? 1 : 0;
        }
        if (enableCount <= 1) {
            if (m_monitors.size()) {
                m_monitors.first()->Enable(true);
            }
            return;
        }
    }
    const bool ismerge = m_model->isMerge();
    auto *primary = m_model->primaryMonitor();
    //如果设置的是主屏，则先改主屏
    if (monitor == primary) {
        for (auto *tm : m_model->monitorList()) {
            // pass primary
            if (tm == monitor || !tm->enable())
                continue;

            setPrimaryByName(tm->name());
            primary = tm;
            break;
        }
    }

    //灭掉屏幕
    MonitorInter *inter = m_monitors.value(monitor);
    if (enabled) {
        auto modes = monitor->modeList();
        Resolution bestMode = modes.first();
        qDebug() << "get best Resolution :" << bestMode.width() << " x " << bestMode.height();
        auto mode = bestMode;
        if (ismerge) {
            Resolution currentmode;
            for (auto *tm : m_model->monitorList()) {
                if (tm->enable()) {
                    currentmode = tm->currentMode();
                    break;
                }
            }
            if (monitor->hasResolutionAndRate(currentmode)) {
                mode = currentmode;
            }
        }

        const auto rotate = m_model->primaryMonitor()->rotate();
        const auto brightness = m_model->primaryMonitor()->brightness();

        QList<QDBusPendingReply<>> replys;
        replys << inter->SetModeBySize(static_cast<ushort>(mode.width()), static_cast<ushort>(mode.height()));
        replys << inter->SetRotation(rotate);
        replys << inter->Enable(enabled);
        replys << m_displayInter->SetBrightness(monitor->name(), brightness);

        //防止customsettingdialog起的时候monitor的属性值不对
        monitor->setW(mode.width());
        monitor->setH(mode.height());

        for (auto r : replys)
            r.waitForFinished();
    } else
        inter->Enable(enabled).waitForFinished();
    Q_ASSERT(m_monitors.contains(primary));
    m_monitors[primary]->SetPosition(0, 0).waitForFinished();

    //为亮的屏幕排序
    int xOffset = primary->w();
    if (ismerge == false) {
        for (auto *mon : m_model->monitorList()) {
            // pass primary
            if (mon == primary)
                continue;
            if (monitor == mon) {
                if(!enabled)
                    continue;
            }
            else {
                if(!mon->enable())
                    continue;
            }
            Q_ASSERT(m_monitors.contains(mon));
            auto *mInter = m_monitors[mon];
            mInter->SetPosition(static_cast<short>(xOffset), 0).waitForFinished();
            monitor->setW(xOffset);
            monitor->setH(0);
            xOffset += mon->w();
        }
    }
    m_displayInter->ApplyChanges().waitForFinished();
}

void DisplayWorker::setColorTemperature(int value)
{
    m_displayInter->SetColorTemperature(value);
}

void DisplayWorker::SetMethodAdjustCCT(int mode)
{
    m_displayInter->SetMethodAdjustCCT(mode);
}
void DisplayWorker::setMonitorResolution(Monitor *mon, const int mode)
{
    MonitorInter *inter = m_monitors.value(mon);
    Q_ASSERT(inter);

    inter->SetMode(static_cast<uint>(mode)).waitForFinished();
}

void DisplayWorker::setMonitorBrightness(Monitor *mon, const double brightness)
{
    m_displayInter->SetAndSaveBrightness(mon->name(), std::max(brightness, m_model->minimumBrightnessScale())).waitForFinished();
}

void DisplayWorker::setMonitorPosition(Monitor *mon, const int x, const int y)
{
    MonitorInter *inter = m_monitors.value(mon);
    Q_ASSERT(inter);

    inter->SetPosition(static_cast<short>(x), static_cast<short>(y)).waitForFinished();
    m_displayInter->ApplyChanges().waitForFinished();
}

void DisplayWorker::setUiScale(const double value)
{
    double rv = value;
    if (rv < 0) rv = m_model->uiScale();

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

    connect(process, static_cast<void (QProcess::*)(int exitCode)>(&QProcess::finished), this, [ = ] {
        process->close();
        process->deleteLater();
    });

    process->start("bash", QStringList() << "-c" << QString("systemctl --user %1 redshift.service && systemctl --user %2 redshift.service")
                   .arg(serverCmd)
                   .arg(cmd));
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

    connect(inter, &MonitorInter::CurrentModeChanged, this,  [ = ] (Resolution  value) {
        if (value.id() == 0) {
            return ;
        }
        auto maxWScale = value.width() / 1024.0;
        auto maxHScale = value.height() / 768.0;
        auto maxScale = maxWScale < maxHScale ? maxWScale : maxHScale;
        if ((m_model->uiScale() - maxScale) > 0.01 && maxScale >= 1.0) {
            double scale =1.0;
            for (int idx = 0; idx * 0.25 + 1.0 <= maxScale; ++idx) {
                scale = idx * 0.25 + 1.0 ;
            }
            setUiScale(scale);
        }
    });
    connect(inter, &MonitorInter::ModesChanged, mon, &Monitor::setModeList);
    connect(inter, &MonitorInter::RotationsChanged, mon, &Monitor::setRotateList);
    connect(inter, &MonitorInter::EnabledChanged, mon, &Monitor::setMonitorEnable);
    connect(m_displayInter, static_cast<void (DisplayInter::*)(const QString &) const>(&DisplayInter::PrimaryChanged), mon, &Monitor::setPrimary);

    // NOTE: DO NOT using async dbus call. because we need to have a unique name to distinguish each monitor
    Q_ASSERT(inter->isValid());
    mon->setName(inter->name());
    mon->setManufacturer(inter->manufacturer());
    mon->setModel(inter->model());
    QDBusReply<bool> reply = m_displayDBusInter->call("CanSetBrightness", inter->name());
    mon->setCanBrightness(reply.value());
    mon->setMonitorEnable(inter->enabled());
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
    mon->setPrimary(m_displayInter->primary());
    mon->setMmWidth(inter->mmWidth());
    mon->setMmHeight(inter->mmHeight());

    if (!m_model->brightnessMap().isEmpty()) {
        mon->setBrightness(m_model->brightnessMap()[mon->name()]);
    }

    m_model->monitorAdded(mon);
    auto getDisplayPriority = [](QString name) {
        if(name.contains("edp",Qt::CaseInsensitive)) {
            return 1;
        }
        else if(name.contains("hdmi",Qt::CaseInsensitive)) {
            return 2;
        }
        else if(name.contains("dvi",Qt::CaseInsensitive)) {
            return 3;
        }
        else if(name.contains("vga",Qt::CaseInsensitive)) {
            return 3;
        }
        else {
            return 4;
        }
    } ;
    if(m_model->displayMode()==EXTEND_MODE) {
        int  curPriority=getDisplayPriority(m_model->primary());
        int  monPriority=getDisplayPriority(mon->name());
        if(monPriority < curPriority) {
             m_displayInter->SetPrimary(mon->name());
        }
    }
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

    if (key == GSETTINGS_MINIMUM_BRIGHTNESS)
        m_model->setMinimumBrightnessScale(value.toDouble());
}

void DisplayWorker::record()
{
    const int displayMode { m_model->displayMode() };
    const QString config { displayMode == CUSTOM_MODE ? m_model->config() : m_model->primary() };
    m_model->setLastConfig(std::pair<int, QString>(displayMode, config));
}

void DisplayWorker::restore()
{
    const std::pair<int, QString> lastConfig { m_model->lastConfig() };
    m_model->setDisplayMode(lastConfig.first);
    switch (lastConfig.first) {
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

void DisplayWorker::setAmbientLightAdjustBrightness(bool able)
{
    m_powerInter->setAmbientLightAdjustBrightness(able);
}

void DisplayWorker::setTouchScreenAssociation(const QString &monitor, const QString &touchscreenSerial)
{
    m_displayInter->AssociateTouch(monitor, touchscreenSerial);
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
