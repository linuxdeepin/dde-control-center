//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "displayworker.h"
#include "displaymodel.h"
#include "widgets/utils.h"

#include <dconfig.h>

#include <QDebug>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(DdcDisplayWorker, "dcc-display-worker")


const QString DisplayInterface("org.deepin.dde.Display1");

Q_DECLARE_METATYPE(QList<QDBusObjectPath>)
using namespace DCC_NAMESPACE;

DisplayWorker::DisplayWorker(DisplayModel *model, QObject *parent, bool isSync)
    : QObject(parent)
    , m_model(model)
    , m_displayInter(new DisplayDBusProxy(this))
    , m_updateScale(false)
    , m_timer(new QTimer(this))
    , m_dconfig(DConfig::create("org.deepin.dde.control-center", QStringLiteral("org.deepin.dde.control-center.display"), QString(), this))
{
    // NOTE: what will it be used?
    Q_UNUSED(isSync)
    m_timer->setSingleShot(true);
    m_timer->setInterval(200);

    connect(m_displayInter, &DisplayDBusProxy::MonitorsChanged, this, &DisplayWorker::onMonitorListChanged);
    connect(m_displayInter, &DisplayDBusProxy::MachinesChanged, this, &DisplayWorker::onMachinesChanged);
    connect(m_displayInter, &DisplayDBusProxy::CooperatedMachinesChanged, this, &DisplayWorker::onHistoryDevChanged);
    connect(m_displayInter, &DisplayDBusProxy::BrightnessChanged, this, &DisplayWorker::onMonitorsBrightnessChanged);
    connect(m_displayInter, &DisplayDBusProxy::BrightnessChanged, model, &DisplayModel::setBrightnessMap);
    connect(m_displayInter, &DisplayDBusProxy::TouchscreensV2Changed, model, &DisplayModel::setTouchscreenList);
    connect(m_displayInter, &DisplayDBusProxy::TouchMapChanged, model, &DisplayModel::setTouchMap);
    connect(m_displayInter, &DisplayDBusProxy::ScreenHeightChanged, model, &DisplayModel::setScreenHeight);
    connect(m_displayInter, &DisplayDBusProxy::ScreenWidthChanged, model, &DisplayModel::setScreenWidth);
    connect(m_displayInter, &DisplayDBusProxy::DisplayModeChanged, model, &DisplayModel::setDisplayMode);
    connect(m_displayInter, &DisplayDBusProxy::MaxBacklightBrightnessChanged, model, &DisplayModel::setmaxBacklightBrightness);
    connect(m_displayInter, &DisplayDBusProxy::ColorTemperatureModeChanged, model, &DisplayModel::setAdjustCCTmode);
    connect(m_displayInter, &DisplayDBusProxy::ColorTemperatureManualChanged, model, &DisplayModel::setColorTemperature);
    connect(m_displayInter, &DisplayDBusProxy::DeviceSharingSwitchChanged, m_model, &DisplayModel::setDeviceSharingSwitch);
    connect(m_displayInter, static_cast<void (DisplayDBusProxy::*)(const QString &) const>(&DisplayDBusProxy::PrimaryChanged), model, &DisplayModel::setPrimary);

    //display redSfit/autoLight
    connect(m_displayInter, &DisplayDBusProxy::HasAmbientLightSensorChanged, m_model, &DisplayModel::autoLightAdjustVaildChanged);
    connect(m_timer, &QTimer::timeout, this, [=] {
        m_displayInter->ApplyChanges().waitForFinished();
        m_displayInter->Save().waitForFinished();
    });
}

DisplayWorker::~DisplayWorker()
{
    qDeleteAll(m_monitors.keys());
    qDeleteAll(m_monitors.values());
}

void DisplayWorker::active()
{
//    m_model->setAllowEnableMultiScaleRatio(
//        valueByQSettings<bool>(DCC_CONFIG_FILES,
//                               "Display",
//                               "AllowEnableMultiScaleRatio",
//                               false));

    QDBusPendingCallWatcher *scalewatcher = new QDBusPendingCallWatcher(m_displayInter->GetScaleFactor());
    connect(scalewatcher, &QDBusPendingCallWatcher::finished, this, &DisplayWorker::onGetScaleFinished);

    QDBusPendingCallWatcher *screenscaleswatcher = new QDBusPendingCallWatcher(m_displayInter->GetScreenScaleFactors());
    connect(screenscaleswatcher, &QDBusPendingCallWatcher::finished, this, &DisplayWorker::onGetScreenScalesFinished);

    onMonitorsBrightnessChanged(m_displayInter->brightness());
    m_model->setBrightnessMap(m_displayInter->brightness());
    onMonitorListChanged(m_displayInter->monitors());
    onMachinesChanged(m_displayInter->Machines());
    onHistoryDevChanged(m_displayInter->CooperatedMachines());

    m_model->setDisplayMode(m_displayInter->displayMode());
    m_model->setTouchscreenList(m_displayInter->touchscreensV2());
    m_model->setTouchMap(m_displayInter->touchMap());
    m_model->setPrimary(m_displayInter->primary());
    m_model->setScreenHeight(m_displayInter->screenHeight());
    m_model->setScreenWidth(m_displayInter->screenWidth());
    m_model->setAdjustCCTmode(m_displayInter->colorTemperatureMode());
    m_model->setColorTemperature(m_displayInter->colorTemperatureManual());
    m_model->setmaxBacklightBrightness(m_displayInter->maxBacklightBrightness());
    m_model->setAutoLightAdjustIsValid(m_displayInter->hasAmbientLightSensor());
    m_model->setDeviceSharingSwitch(m_displayInter->deviceSharingSwitch());
    m_model->setOpenSharedDevices(m_displayInter->sharedDevices());
    m_model->setOpenSharedClipboard(m_displayInter->sharedClipboard());
    m_model->setFilesStoragePath(m_displayInter->filesStoragePath());
    connect(m_displayInter, &DisplayDBusProxy::SharedClipboardChanged, m_model, &DisplayModel::setOpenSharedClipboard);
    connect(m_displayInter, &DisplayDBusProxy::SharedDevicesChanged, m_model, &DisplayModel::setOpenSharedDevices);
    connect(m_displayInter, &DisplayDBusProxy::FilesStoragePathChanged, m_model, &DisplayModel::setFilesStoragePath);

    bool isRedshiftValid = true;
    QDBusReply<bool> reply = m_displayInter->SupportSetColorTemperatureSync();
    if (QDBusError::NoError == reply.error().type())
        isRedshiftValid = reply.value();
    else
        qCWarning(DdcDisplayWorker) << "Call SupportSetColorTemperature method failed: " << reply.error().message();
    m_model->setRedshiftIsValid(isRedshiftValid);
    QVariant minBrightnessValue = 0.1f;
    minBrightnessValue = m_dconfig->value("minBrightnessValue", minBrightnessValue);
    m_model->setMinimumBrightnessScale(minBrightnessValue.toDouble());
//    m_model->setResolutionRefreshEnable(m_dccSettings->get(GSETTINGS_SHOW_MUTILSCREEN).toBool());
//    m_model->setBrightnessEnable(m_dccSettings->get(GSETTINGS_BRIGHTNESS_ENABLE).toBool());
}

void DisplayWorker::saveChanges()
{
    clearBackup();
    m_displayInter->Save().waitForFinished();
    if (m_updateScale)
        setUiScale(m_currentScale);
    m_updateScale = false;
}

void DisplayWorker::switchMode(const int mode, const QString &name)
{
    m_displayInter->SwitchMode(static_cast<uchar>(mode), name).waitForFinished();
}

void DisplayWorker::onMonitorListChanged(const QList<QDBusObjectPath> &mons)
{
    QList<QString> ops;
    for (const auto *mon : m_monitors.keys())
        ops << mon->path();

    qCDebug(DdcDisplayWorker) << mons.size();
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

void DisplayWorker::onMachinesChanged(const QList<QDBusObjectPath> &machines)
{
    // TODO: 获取协同接口 同步协同设备
    QList<QString> machList;
    QList<QString> existMachines;  //存在的Machine

    for (auto&& mon : m_machines.keys()) {
        existMachines.append(mon->Path());
    }

    for (const auto &ma : machines) {
        const QString path = ma.path();
        machList << path;
        if (!existMachines.contains(path))
            machinesAdded(path);
    }

    for (const auto &ma : existMachines) {
        if (!machList.contains(ma)) {
            machinesRemoved(ma);
        }
    }
}

void DisplayWorker::onHistoryDevChanged(const QList<QString> &machines)
{
    for (const auto &hisdevPath : machines) {
        const QString path = hisdevPath;
        for (auto&& machine : m_machines.keys()) {
            machine->setHistoryStates(machine->UUID() == path);
        }
    }
}

#ifndef DCC_DISABLE_ROTATE
void DisplayWorker::setMonitorRotate(Monitor *mon, const quint16 rotate)
{
    if (m_model->displayMode() == MERGE_MODE) {
        for (auto *m : m_monitors) {
            m->SetRotation(rotate).waitForFinished();
        }
    } else {
        MonitorDBusProxy *inter = m_monitors.value(mon);
        inter->SetRotation(rotate).waitForFinished();
    }
}
#endif

void DisplayWorker::setPrimary(const QString &name)
{
    m_displayInter->SetPrimary(name);
}

void DisplayWorker::setMonitorEnable(Monitor *monitor, const bool enable)
{
    MonitorDBusProxy *inter = m_monitors.value(monitor);
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
    m_displayInter->SetColorTemperature(value).waitForFinished();
}

void DisplayWorker::SetMethodAdjustCCT(int mode)
{
    m_displayInter->SetMethodAdjustCCT(mode);
}

void DisplayWorker::setCurrentFillMode(Monitor *mon,const QString fillMode)
{
    MonitorDBusProxy *inter = m_monitors.value(mon);
    Q_ASSERT(inter);
    inter->setCurrentFillMode(fillMode);
}

void DisplayWorker::setDeviceSharingSwitch(const bool enable)
{
     m_displayInter->setDeviceSharingSwitch(enable);
}

void DisplayWorker::setCurrentMachineConnect(Machine *mac)
{
    qCDebug(DdcDisplayWorker) << " 设置Connect： " << mac->Name();
    MachineDBusProxy *inter = m_machines.value(mac);
    inter->connect();
}

void DisplayWorker::setCurrentRequestDeviceSharing(Machine *mac)
{
    qCDebug(DdcDisplayWorker) << " 设置 DeviceSharing： " << mac->Name();
    MachineDBusProxy *inter = m_machines.value(mac);
    inter->requestDeviceSharing();
}

void DisplayWorker::setCurrentMachineDisconnect(Machine *mac)
{
    MachineDBusProxy *inter = m_machines.value(mac);
    inter->disconnect();
}

void DisplayWorker::setCurrentStopDeviceSharing(Machine *mac)
{
    MachineDBusProxy *inter = m_machines.value(mac);
    inter->stopDeviceSharing();
}

void DisplayWorker::setOpenSharedDevices(bool on)
{
    m_displayInter->setOpenSharedDevices(on);
}

void DisplayWorker::setOpenSharedClipboard(bool on)
{
    m_displayInter->setOpenSharedClipboard(on);
}

void DisplayWorker::setFilesStoragePath(const QString &path)
{
    m_displayInter->setFilesStoragePath(path);
}

void DisplayWorker::setFlowDirection(Machine *mac, const int &dir)
{
    MachineDBusProxy *inter = m_machines.value(mac);
    inter->setFlowDirection(dir);
}

void DisplayWorker::backupConfig()
{
    m_displayConfig = m_displayInter->GetConfig();
}

void DisplayWorker::clearBackup()
{
    m_displayConfig.clear();
}

void DisplayWorker::resetBackup()
{
    if (!m_displayConfig.isEmpty()) {

        QJsonDocument doc = QJsonDocument::fromJson(m_displayConfig.toLatin1());
        QJsonObject jsonObj = doc.object();

        QDateTime time = QDateTime::currentDateTime();
        int offset = time.offsetFromUtc()/60;
        bool negative = offset <0;
        if (negative)
            offset = -offset;

        jsonObj.insert("UpdateAt", QString("%1%2%3:%4").arg(time.toString("yyyy-MM-ddThh:mm:ss.zzz000000")).arg(negative ? '-' : '+').arg(offset / 60, 2, 10, QChar('0')).arg(offset % 60, 2, 10, QChar('0')));
        doc.setObject(jsonObj);

        m_displayInter->SetConfig(doc.toJson(QJsonDocument::Compact));
        clearBackup();
    }
}

void DisplayWorker::setMonitorResolution(Monitor *mon, const int mode)
{
    MonitorDBusProxy *inter = m_monitors.value(mon);
    if (inter)
        inter->SetMode(static_cast<uint>(mode)).waitForFinished();
}

void DisplayWorker::setMonitorBrightness(Monitor *mon, const double brightness)
{
    m_displayInter->SetAndSaveBrightness(mon->name(), std::max(brightness, m_model->minimumBrightnessScale())).waitForFinished();
}

void DisplayWorker::setMonitorPosition(QHash<Monitor *, QPair<int, int>> monitorPosition)
{
    for (auto it(monitorPosition.cbegin()); it != monitorPosition.cend(); ++it) {
        MonitorDBusProxy *inter = m_monitors.value(it.key());
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
    QDBusPendingCall call = m_displayInter->SetScaleFactor(rv);

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
    m_displayInter->SetScreenScaleFactors(scalemap);
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

    connect(process, static_cast<void (QProcess::*)(int exitCode, QProcess::ExitStatus)>(&QProcess::finished), process, &QProcess::deleteLater);

    process->start("bash", QStringList() << "-c" << QString("systemctl --user %1 redshift.service && systemctl --user %2 redshift.service").arg(serverCmd).arg(cmd));
}

void DisplayWorker::monitorAdded(const QString &path)
{
    MonitorDBusProxy *inter = new MonitorDBusProxy(path, this);
    Monitor *mon = new Monitor(this);

    connect(inter, &MonitorDBusProxy::XChanged, mon, &Monitor::setX);
    connect(inter, &MonitorDBusProxy::YChanged, mon, &Monitor::setY);
    connect(inter, &MonitorDBusProxy::WidthChanged, mon, &Monitor::setW);
    connect(inter, &MonitorDBusProxy::HeightChanged, mon, &Monitor::setH);
    connect(inter, &MonitorDBusProxy::MmWidthChanged, mon, &Monitor::setMmWidth);
    connect(inter, &MonitorDBusProxy::MmHeightChanged, mon, &Monitor::setMmHeight);
    connect(inter, &MonitorDBusProxy::RotationChanged, mon, &Monitor::setRotate);
    connect(inter, &MonitorDBusProxy::NameChanged, mon, &Monitor::setName);
    connect(inter, &MonitorDBusProxy::CurrentModeChanged, mon, &Monitor::setCurrentMode);
    connect(inter, &MonitorDBusProxy::BestModeChanged, mon, &Monitor::setBestMode);
    connect(inter, &MonitorDBusProxy::CurrentModeChanged, this, [=](Resolution value) {
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

    connect(inter, &MonitorDBusProxy::ModesChanged, mon, &Monitor::setModeList);
    connect(inter, &MonitorDBusProxy::RotationsChanged, mon, &Monitor::setRotateList);
    connect(inter, &MonitorDBusProxy::EnabledChanged, mon, &Monitor::setMonitorEnable);
    connect(inter, &MonitorDBusProxy::CurrentRotateModeChanged, mon, &Monitor::setCurrentRotateMode);
    connect(inter, &MonitorDBusProxy::AvailableFillModesChanged, mon, &Monitor::setAvailableFillModes);
    connect(inter, &MonitorDBusProxy::CurrentFillModeChanged, mon, &Monitor::setCurrentFillMode);
    connect(m_displayInter, static_cast<void (DisplayDBusProxy::*)(const QString &) const>(&DisplayDBusProxy::PrimaryChanged), mon, &Monitor::setPrimary);
    connect(this, &DisplayWorker::requestUpdateModeList, this, [=] {
        mon->setModeList(inter->modes());
    });

    // NOTE: DO NOT using async dbus call. because we need to have a unique name to distinguish each monitor
    mon->setName(inter->name());
    mon->setManufacturer(inter->manufacturer());
    mon->setModel(inter->model());
    QDBusReply<bool> reply = m_displayInter->CanSetBrightnessSync(inter->name());
    mon->setCanBrightness(reply.value());
    mon->setMonitorEnable(inter->enabled());
    mon->setCurrentRotateMode(inter->currentRotateMode());
    mon->setMonitorEnable(inter->enabled());
    mon->setCurrentFillMode(inter->currentFillMode());
    mon->setAvailableFillModes(inter->availableFillModes());
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
    m_monitors.insert(mon, inter);
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

void DisplayWorker::machinesAdded(const QString &path)
{
    MachineDBusProxy *interProxy = new MachineDBusProxy(path, this);
    Machine *machine = new Machine(this);

    connect(interProxy, &MachineDBusProxy::IPChanged, machine, &Machine::setIP);
    connect(interProxy, &MachineDBusProxy::NameChanged, machine, &Machine::setName);
    connect(interProxy, &MachineDBusProxy::ConnectedChanged, machine, &Machine::setConnected);
    connect(interProxy, &MachineDBusProxy::DeviceSharingChanged, machine, &Machine::setDeviceSharing);
    connect(interProxy, &MachineDBusProxy::disconnectStatusChanged, machine, &Machine::setDisconnectStatus);
    connect(interProxy, &MachineDBusProxy::DirectionChanged, machine, &Machine::setDirection);

    machine->setPath(path);
    machine->setIP(interProxy->ip());
    machine->setName(interProxy->name());
    machine->setConnected(interProxy->connected());
    machine->setDeviceSharing(interProxy->deviceSharing());
    machine->setUUID(interProxy->uuid());
    machine->setDirection(interProxy->direction());

    // 标记历史设备
    const QList<QString> &historyDev = m_displayInter->CooperatedMachines();
    for (auto &hisdevPath : historyDev) {
        const QString path = hisdevPath;
        qCDebug(DdcDisplayWorker) << " hisdevPath UUID： " << path << machine->UUID();
        if (machine->UUID() == path)
            machine->setHistoryStates(true);
    }

    m_model->machinesAdded(machine);
    m_machines.insert(machine, interProxy);
}

void DisplayWorker::machinesRemoved(const QString &path)
{
    Machine *machine = nullptr;
    for (auto it(m_machines.cbegin()); it !=m_machines.cend(); ++it) {
        if (it.key()->Path() == path) {
            machine = it.key();
            break;
        }
    }

    if (!machine)
        return;

    m_model->machinesRemoved(machine);
    m_machines[machine]->deleteLater();
    m_machines.remove(machine);

    machine->deleteLater();
}

void DisplayWorker::setAmbientLightAdjustBrightness(bool able)
{
    m_displayInter->setAmbientLightAdjustBrightness(able);
}

void DisplayWorker::setTouchScreenAssociation(const QString &monitor, const QString &touchscreenUUID)
{
    m_displayInter->AssociateTouch(monitor, touchscreenUUID);
}

void DisplayWorker::setMonitorResolutionBySize(Monitor *mon, const int width, const int height)
{
    MonitorDBusProxy *inter = m_monitors.value(mon);
    Q_ASSERT(inter);

    QDBusPendingCall call = inter->SetModeBySize(static_cast<ushort>(width), static_cast<ushort>(height));
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            qCDebug(DdcDisplayWorker) << call.error().message();
        }
        watcher->deleteLater();
    });
    watcher->waitForFinished();
}
