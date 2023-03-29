// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "displayworker.h"
#include "displaymodel.h"
#include "widgets/utils.h"

#include <DApplicationHelper>

#include <QDebug>
#include <QtConcurrent>

using namespace dcc;
using namespace dcc::display;

#define GSETTINGS_MINIMUM_BRIGHTNESS "brightness-minimum"
#define GSETTINGS_SHOW_MUTILSCREEN "show-multiscreen"
#define GSETTINGS_BRIGHTNESS_ENABLE "brightness-enable"

const QString DisplayInterface("com.deepin.daemon.Display");
const QString DisplaySystemInterface("com.deepin.system.Display");
const QString DisplaySystemPath("/com/deepin/system/Display");

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

    m_displaySystenInterface = new QDBusInterface(DisplaySystemInterface,
                                                  DisplaySystemPath,
                                                  "org.freedesktop.DBus.Properties",
                                                  QDBusConnection::systemBus());

    //1.先获取是否支持自动调节背光 -> 2.获取内建屏，有内建屏再获取状态 -> 3.获取自动调节亮度状态
    QDBusMessage reply = m_displaySystenInterface->call("Get", DisplaySystemInterface, "SupportLabc");
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "com.deepin.system.Display SupportLabc, QDBusMessage::ErrorMessage.";
        return;
    }
    QList<QVariant> outArgs = reply.arguments();
    if (outArgs.count() <= 0)
        return;

    bool supportLabc = outArgs.at(0).toBool();
    qInfo() << Q_FUNC_INFO << "com.deepin.daemon.Display.GetBuiltinMonitor : " << supportLabc;
    m_model->setSupportLabc(supportLabc);

    //对于不支持“自动亮度调节”后面流程不需要处理
    if (!supportLabc) {
        return;
    }

    //2.获取内建屏
    reply = m_displayInter.call("GetBuiltinMonitor");
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "com.deepin.daemon.Display GetBuiltinMonitor, QDBusMessage::ErrorMessage.";
        return;
    }
    outArgs = reply.arguments();
    if (outArgs.count() <= 0)
        return;

    QString value  = outArgs.at(0).toString();
    qInfo() << Q_FUNC_INFO << "com.deepin.daemon.Display.GetBuiltinMonitor : " << value;
    //仅对支持内建屏的机器进行处理
    if (m_model && value != "") {
        m_model->setBuiltinMonitor(value);

        //3.获取自动调节亮度状态
        reply = m_displaySystenInterface->call("Get", DisplaySystemInterface, "AutoBacklightEnabled");
        if (reply.type() == QDBusMessage::ErrorMessage) {
            qWarning() << "com.deepin.system.Display AutoBacklightEnabled, QDBusMessage::ErrorMessage.";
            return;
        }
        outArgs = reply.arguments();
        if (outArgs.count() <= 0)
            return;
        bool autoBacklightEnabled = outArgs.at(0).toBool();
        m_model->setAutoBacklightEnabled(autoBacklightEnabled);

        QDBusConnection::systemBus().connect(DisplaySystemInterface, DisplaySystemPath,
                                             "org.freedesktop.DBus.Properties", "PropertiesChanged",
                                             "sa{sv}as", this, SLOT(handlePropertiesChanged(QDBusMessage)));

        connect(m_model, &DisplayModel::notifyUserSetAutoBacklightEnabledChanged, this, &DisplayWorker::setAutoBacklightEnabled);
    } else {
        qWarning() << Q_FUNC_INFO << m_model << value;
    }
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

    bool isRedshiftValid = true;
    QDBusInterface displayInter("com.deepin.daemon.Display","/com/deepin/daemon/Display",
            "com.deepin.daemon.Display", QDBusConnection::sessionBus());
    QDBusReply<bool> reply = displayInter.call("SupportSetColorTemperature");
    if (QDBusError::NoError == reply.error().type())
        isRedshiftValid = reply.value();
    else
        qWarning() << "Call SupportSetColorTemperature method failed: " << reply.error().message();
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
    m_displayInter.SwitchMode(static_cast<uchar>(mode), name).waitForFinished();;
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

void DisplayWorker::handlePropertiesChanged(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
    if (3 != arguments.count()) {
        return;
    }

    QString interfaceName = msg.arguments().at(0).toString();
    if (interfaceName == DisplaySystemInterface) {
        QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
        QStringList keys = changedProps.keys();
        for (int i = 0; i < keys.size(); i++) {
            if (keys.at(i) == "AutoBacklightEnabled") {
                bool autoBacklightEnabled = changedProps.value(keys.at(i)).toBool();
                qInfo() << Q_FUNC_INFO << "com.deepin.system.Display PropertiesChanged AutoBacklightEnabled : " << autoBacklightEnabled;
                m_model->setAutoBacklightEnabled(autoBacklightEnabled);
                return;
            }
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
        MonitorInter *inter = m_monitors.value(mon);
        inter->SetRotation(rotate).waitForFinished();
    }
}
#endif

void DisplayWorker::setPrimary(const QString &name)
{
    // 延时调用，避免卡在下拉框未收回时的一帧画面
    QTimer::singleShot(150, this, [=] {
        m_displayInter.SetPrimary(name);
    });
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

void DisplayWorker::setCurrentFillMode(Monitor *mon,const QString fillMode)
{
    MonitorInter *inter = m_monitors.value(mon);
    Q_ASSERT(inter);
    inter->setCurrentFillMode(fillMode);
}

void DisplayWorker::setAutoBacklightEnabled(const bool value)
{
    if (!m_displaySystenInterface) {
        return;
    }
    QDBusMessage reply = m_displaySystenInterface->call("Set", DisplaySystemInterface, "AutoBacklightEnabled", QVariant::fromValue(QDBusVariant(value)));
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "com.deepin.system.Display set AutoBacklightEnabled, QDBusMessage::ErrorMessage.";
    } else {
        qInfo() << "com.deepin.system.Display set AutoBacklightEnabled, value : " << value;
    }
}

void DisplayWorker::setMonitorResolution(Monitor *mon, const int mode)
{
    MonitorInter *inter = m_monitors.value(mon);
    Q_ASSERT(inter);
    inter->SetMode(static_cast<uint>(mode)).waitForFinished();
}

void DisplayWorker::setMonitorBrightness(Monitor *mon, const double brightness)
{
    // m_displayInter.SetAndSaveBrightness(mon->name(), std::max(brightness, m_model->minimumBrightnessScale())).waitForFinished();
    double value = std::max(brightness, m_model->minimumBrightnessScale());
    qDebug() << "setMonitorBrightness: receive request" << mon->name() << value;

    //前面亮度设置未完成，只记录最新请求
    QMutexLocker loker(&m_brightness.m_brightnessMutex);
    m_brightness.m_hasWaitingRequest = true;
    m_brightness.m_brightnessValue = value;
    m_brightness.m_monitorName = mon->name();
    if (!m_brightness.m_hasPendingRequest) {
        QTimer::singleShot(0, this, &DisplayWorker::handleSetBrightnessRequest);
    }
}

void DisplayWorker::handleSetBrightnessRequest() {
    qDebug() << "setMonitorBrightness enter";
    QMutexLocker loker(&m_brightness.m_brightnessMutex);
    if (m_brightness.m_hasPendingRequest) {
        return;
    }
    if (!m_brightness.m_hasWaitingRequest) {
        return;
    }

    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<QDBusPendingReply<>>::finished, [this, watcher] {
        QMutexLocker loker(&m_brightness.m_brightnessMutex);
        m_brightness.m_hasPendingRequest = false;
        qDebug() << "setMonitorBrightness: finish";
        if (m_brightness.m_hasWaitingRequest) {
            QTimer::singleShot(0, this, &DisplayWorker::handleSetBrightnessRequest);
        }
        watcher->deleteLater();
    });

    QFuture<void> future = QtConcurrent::run([this] {
        QMutexLocker loker(&m_brightness.m_brightnessMutex);
        if (m_brightness.m_hasPendingRequest) {
            return;
        }
        if (!m_brightness.m_hasWaitingRequest) {
            return;
        }
        m_brightness.m_hasPendingRequest = true;
        m_brightness.m_hasWaitingRequest = false;
        QString name = m_brightness.m_monitorName;
        double value = m_brightness.m_brightnessValue;
        m_brightness.m_monitorName = "";
        m_brightness.m_brightnessValue = 0;
        loker.unlock();
        qDebug() << "setMonitorBrightness: begin, " << name << value;
        m_displayInter.SetAndSaveBrightness(name, value).waitForFinished();
    });

    watcher->setFuture(future);
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
    connect(inter, &MonitorInter::AvailableFillModesChanged, mon, &Monitor::setAvailableFillModes);
    connect(inter, &MonitorInter::CurrentFillModeChanged, mon, &Monitor::setCurrentFillMode);
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
