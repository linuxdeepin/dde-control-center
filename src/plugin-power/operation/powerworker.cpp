//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "powerworker.h"
#include "operation/powerdbusproxy.h"
#include "powermodel.h"
#include "utils.h"

#include <QProcessEnvironment>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QRegularExpression>

#define POWER_CAN_SLEEP "POWER_CAN_SLEEP"
#define POWER_CAN_HIBERNATE "POWER_CAN_HIBERNATE"

#define BATTERY_LOCKDELAY_NAME "batteryLockDelay"
#define BATTERY_SLEEPDELAY_NAME "batterySleepDelay"
#define BATTERY_SBDELAY_NAME "batteryScreenBlackDelay"
#define LINEPOWER_LOCKDELAY_NAME "linePowerLockDelay"
#define LINEPOWER_SLEEPDELAY_NAME "linePowerSleepDelay"
#define LINEPOWER_SBDELAY_NAME "linePowerScreenBlackDelay"
#define TIME_WEEK_START_NAME "firstDayOfWeek"
#define ENABLE_SCHEDULED_SHUTDOWN "enableScheduledShutdown"

#define SHOW_HIBERNATE_NAME "showHibernate"
#define SHOW_SHUTDOWN_NAME "showShutdown"
#define SHOW_SUSPEND_NAME "showSuspend"

static const QStringList DCC_CONFIG_FILES {
    "/etc/deepin/dde-control-center.conf",
    "/usr/share/dde-control-center/dde-control-center.conf"
};

PowerWorker::PowerWorker(PowerModel *model, QObject *parent)
    : QObject(parent)
    , m_powerModel(model)
    , m_powerDBusProxy(new PowerDBusProxy(this))
    , m_cfgDock(DConfig::create("org.deepin.dde.tray-loader", "org.deepin.dde.dock.plugin.power", QString(), this))
    , m_cfgPower(DConfig::create("org.deepin.dde.control-center", "org.deepin.dde.control-center.power", QString(), this))
    , m_cfgTime(DConfig::createGeneric("org.deepin.region-format", QString(), this))
{
    connect(m_powerDBusProxy, &PowerDBusProxy::noPasswdLoginChanged, m_powerModel, &PowerModel::setNoPasswdLogin);
    connect(m_powerDBusProxy, &PowerDBusProxy::ScreenBlackLockChanged, m_powerModel, &PowerModel::setScreenBlackLock);
    connect(m_powerDBusProxy, &PowerDBusProxy::SleepLockChanged, m_powerModel, &PowerModel::setSleepLock);
    connect(m_powerDBusProxy, &PowerDBusProxy::LidIsPresentChanged, m_powerModel, &PowerModel::setLidPresent);
    connect(m_powerDBusProxy, &PowerDBusProxy::LidClosedSleepChanged, m_powerModel, &PowerModel::setSleepOnLidOnPowerClose);
    connect(m_powerDBusProxy, &PowerDBusProxy::LinePowerScreenBlackDelayChanged, this, &PowerWorker::setScreenBlackDelayToModelOnPower);
    connect(m_powerDBusProxy, &PowerDBusProxy::LinePowerSleepDelayChanged, this, &PowerWorker::setSleepDelayToModelOnPower);
    connect(m_powerDBusProxy, &PowerDBusProxy::BatteryScreenBlackDelayChanged, this, &PowerWorker::setScreenBlackDelayToModelOnBattery);
    connect(m_powerDBusProxy, &PowerDBusProxy::BatterySleepDelayChanged, this, &PowerWorker::setSleepDelayToModelOnBattery);
    connect(m_powerDBusProxy, &PowerDBusProxy::BatteryLockDelayChanged, this, &PowerWorker::setResponseBatteryLockScreenDelay);
    connect(m_powerDBusProxy, &PowerDBusProxy::LinePowerLockDelayChanged, this, &PowerWorker::setResponsePowerLockScreenDelay);
    connect(m_powerDBusProxy, &PowerDBusProxy::IsHighPerformanceSupportedChanged, this, &PowerWorker::setHighPerformanceSupported);

    connect(m_powerDBusProxy, &PowerDBusProxy::PowerSavingModeAutoChanged, m_powerModel, &PowerModel::setAutoPowerSaveMode);
    connect(m_powerDBusProxy, &PowerDBusProxy::PowerSavingModeEnabledChanged, m_powerModel, &PowerModel::setPowerSaveMode);

    connect(m_powerDBusProxy, &PowerDBusProxy::HasBatteryChanged, m_powerModel, &PowerModel::setHaveBettary);

    //--------------------sp2 add----------------------------
    connect(m_powerDBusProxy, &PowerDBusProxy::PowerSavingModeAutoWhenBatteryLowChanged, m_powerModel, &PowerModel::setPowerSavingModeAutoWhenQuantifyLow);
    connect(m_powerDBusProxy, &PowerDBusProxy::PowerSavingModeAutoChanged, m_powerModel, &PowerModel::setPowerSavingModeAuto);
    connect(m_powerDBusProxy, &PowerDBusProxy::PowerSavingModeBrightnessDropPercentChanged, m_powerModel, &PowerModel::setPowerSavingModeLowerBrightnessThreshold);
    connect(m_powerDBusProxy, &PowerDBusProxy::PowerSavingModeAutoBatteryPercentChanged, m_powerModel, &PowerModel::setPowerSavingModeAutoBatteryPercentage);
    connect(m_powerDBusProxy, &PowerDBusProxy::LinePowerPressPowerBtnActionChanged, m_powerModel, &PowerModel::setLinePowerPressPowerBtnAction);
    connect(m_powerDBusProxy, &PowerDBusProxy::LinePowerLidClosedActionChanged, m_powerModel, &PowerModel::setLinePowerLidClosedAction);
    connect(m_powerDBusProxy, &PowerDBusProxy::BatteryPressPowerBtnActionChanged, m_powerModel, &PowerModel::setBatteryPressPowerBtnAction);
    connect(m_powerDBusProxy, &PowerDBusProxy::BatteryLidClosedActionChanged, m_powerModel, &PowerModel::setBatteryLidClosedAction);
    connect(m_powerDBusProxy, &PowerDBusProxy::LowPowerNotifyEnableChanged, m_powerModel, &PowerModel::setLowPowerNotifyEnable);
    connect(m_powerDBusProxy, &PowerDBusProxy::LowPowerNotifyThresholdChanged, m_powerModel, &PowerModel::setLowPowerNotifyThreshold);
    connect(m_powerDBusProxy, &PowerDBusProxy::LowPowerAutoSleepThresholdChanged, m_powerModel, &PowerModel::setLowPowerAutoSleepThreshold);
    connect(m_powerDBusProxy, &PowerDBusProxy::ScheduledShutdownStateChanged, m_powerModel, &PowerModel::setScheduledShutdownState);
    connect(m_powerDBusProxy, &PowerDBusProxy::ShutdownTimeChanged, m_powerModel, &PowerModel::setShutdownTime);
    connect(m_powerDBusProxy, &PowerDBusProxy::ShutdownRepetitionChanged, m_powerModel, &PowerModel::setShutdownRepetition);
    connect(m_powerDBusProxy, &PowerDBusProxy::CustomShutdownWeekDaysChanged, this, &PowerWorker::onCustomShutdownWeekDaysChanged);
    //-------------------------------------------------------
    connect(m_powerDBusProxy, &PowerDBusProxy::ModeChanged, m_powerModel, &PowerModel::setPowerPlan);
    connect(m_powerDBusProxy, &PowerDBusProxy::BatteryCapacityChanged, m_powerModel, &PowerModel::setBatteryCapacity);
    connect(m_powerDBusProxy, &PowerDBusProxy::LowPowerActionChanged, m_powerModel, &PowerModel::setLowPowerAction);

    connect(m_cfgDock, &DConfig::valueChanged, [this] (const QString &key) {
        if ("showTimeToFull" == key) {
            m_powerModel->setShowBatteryTimeToFull(m_cfgDock->value("showTimeToFull").toBool());
        }
    });
  
    connect(m_cfgPower, &DConfig::valueChanged, [this](const QString &key) {
        if (key == BATTERY_LOCKDELAY_NAME) {
            readConfig(BATTERY_LOCKDELAY_NAME,
                std::bind(&PowerModel::setBatteryLockDelayModel, m_powerModel, std::placeholders::_1));
        } else if (key == BATTERY_SLEEPDELAY_NAME) {
            readConfig(BATTERY_SLEEPDELAY_NAME,
                std::bind(&PowerModel::setBatterySleepDelayModel, m_powerModel, std::placeholders::_1));
        } else if (key == BATTERY_SBDELAY_NAME) {
            readConfig(BATTERY_SBDELAY_NAME,
                std::bind(&PowerModel::setBatteryScreenBlackDelayModel, m_powerModel, std::placeholders::_1));
        } else if (key == LINEPOWER_LOCKDELAY_NAME) {
            readConfig(LINEPOWER_LOCKDELAY_NAME,
                std::bind(&PowerModel::setLinePowerLockDelayModel, m_powerModel, std::placeholders::_1));
        } else if (key == LINEPOWER_SLEEPDELAY_NAME) {
            readConfig(LINEPOWER_SLEEPDELAY_NAME,
                std::bind(&PowerModel::setLinePowerSleepDelayModel, m_powerModel, std::placeholders::_1));
        } else if (key == LINEPOWER_SBDELAY_NAME) {
            readConfig(LINEPOWER_SBDELAY_NAME,
                std::bind(&PowerModel::setLinePowerScreenBlackDelayModel, m_powerModel, std::placeholders::_1));
        } else if (key == SHOW_HIBERNATE_NAME) {
            readConfig(SHOW_HIBERNATE_NAME,
                std::bind(&PowerModel::setHibernate, m_powerModel, std::placeholders::_1));
        } else if (key == SHOW_SHUTDOWN_NAME) {
            readConfig(SHOW_SHUTDOWN_NAME,
                std::bind(&PowerModel::setShutdown, m_powerModel, std::placeholders::_1));
        } else if (key == SHOW_SUSPEND_NAME) {
            readConfig(SHOW_SUSPEND_NAME,
                std::bind(&PowerModel::setSuspend, m_powerModel, std::placeholders::_1));
        } else if (key == ENABLE_SCHEDULED_SHUTDOWN) {
            readConfig(ENABLE_SCHEDULED_SHUTDOWN,
                std::bind(&PowerModel::setEnableScheduledShutdown, m_powerModel, std::placeholders::_1));
        }
    });

    connect(m_cfgTime, &DConfig::valueChanged, [this](const QString &key) {
        if (key == TIME_WEEK_START_NAME) {
            int value = m_cfgTime->value(TIME_WEEK_START_NAME).toInt();
            m_powerModel->setWeekBegins(value);
        }
    });

    // init base property
    m_powerModel->setHaveBettary(m_powerDBusProxy->hasBattery());
}

QVariantList PowerWorker::converToDataMap(const QStringList& conf)
{
    QVariantList dataMap;
    for(const QString& numStr : conf) {
        int num;
        if (numStr.isEmpty()) {
            qWarning() << "Convert to num failed, config is empty";
            continue;
        }
        bool ok;
        num = numStr.mid(0, numStr.length() - 1).toInt(&ok);
        if (!ok) {
            qWarning() << "Convert to num failed, can't change num to int";
            num = 0;
        }

        QVariantMap map;
        if (numStr.contains("m")) {
            num = num * 60;
            map["trText"] = numStr.chopped(1) + " " + tr("Minutes");
        } else if (numStr.contains("h")) {
            num = num * 3600;
            map["trText"] = numStr.chopped(1) + " " + tr("Hour");
        }

        map["text"] = numStr;
        map["value"] = num;
        dataMap.push_back(map);
    }
    QVariantMap map;
    map["trText"] = tr("Never");
    map["text"] = tr("Never");
    map["value"] = 0;
    dataMap.push_back(map);
    return dataMap;
}

void PowerWorker::active()
{
    m_powerDBusProxy->blockSignals(false);

    // refersh data
    m_powerModel->setScreenBlackLock(m_powerDBusProxy->screenBlackLock());
    m_powerModel->setSleepLock(m_powerDBusProxy->sleepLock());
    m_powerModel->setLidPresent(m_powerDBusProxy->lidIsPresent());
    m_powerModel->setSleepOnLidOnPowerClose(m_powerDBusProxy->lidClosedSleep());
    m_powerModel->setHaveBettary(m_powerDBusProxy->hasBattery());
    m_powerModel->setPowerSavingModeAutoWhenQuantifyLow(m_powerDBusProxy->powerSavingModeAutoWhenBatteryLow());
    m_powerModel->setPowerSavingModeAutoBatteryPercentage(m_powerDBusProxy->powerSavingModeAutoBatteryPercent());
    m_powerModel->setPowerSavingModeLowerBrightnessThreshold(m_powerDBusProxy->powerSavingModeBrightnessDropPercent());
    m_powerModel->setLowPowerNotifyEnable(m_powerDBusProxy->lowPowerNotifyEnable());
    m_powerModel->setLowPowerAutoSleepThreshold(m_powerDBusProxy->lowPowerAutoSleepThreshold());
    m_powerModel->setLowPowerNotifyThreshold(m_powerDBusProxy->lowPowerNotifyThreshold());
    m_powerModel->setLinePowerPressPowerBtnAction(m_powerDBusProxy->linePowerPressPowerBtnAction());
    m_powerModel->setLinePowerLidClosedAction(m_powerDBusProxy->linePowerLidClosedAction());
    m_powerModel->setBatteryPressPowerBtnAction(m_powerDBusProxy->batteryPressPowerBtnAction());
    m_powerModel->setBatteryLidClosedAction(m_powerDBusProxy->batteryLidClosedAction());
    m_powerModel->setPowerPlan(m_powerDBusProxy->mode());
    m_powerModel->setBatteryCapacity(m_powerDBusProxy->batteryCapacity());
    m_powerModel->setNoPasswdLogin(m_powerDBusProxy->noPasswdLogin());
    m_powerModel->setShowBatteryTimeToFull(m_cfgDock->value("showTimeToFull").toBool());
    m_powerModel->setNoPasswdLogin(m_powerDBusProxy->noPasswdLogin());
    m_powerModel->setScheduledShutdownState(m_powerDBusProxy->scheduledShutdownState());
    m_powerModel->setShutdownTime(m_powerDBusProxy->shutdownTime());
    m_powerModel->setShutdownRepetition(m_powerDBusProxy->shutdownRepetition());
    m_powerModel->setWeekBegins(m_cfgTime->value(TIME_WEEK_START_NAME).toInt());
    m_powerModel->setLowPowerAction(m_powerDBusProxy->lowPowerAction());

    onCustomShutdownWeekDaysChanged(m_powerDBusProxy->customShutdownWeekDays());

    setHighPerformanceSupported(m_powerDBusProxy->isHighPerformanceSupported());
    setBalancePerformanceSupported(m_powerDBusProxy->isBalancePerformanceSupported());

    setScreenBlackDelayToModelOnPower(m_powerDBusProxy->linePowerScreenBlackDelay());
    setSleepDelayToModelOnPower(m_powerDBusProxy->linePowerSleepDelay());

    setScreenBlackDelayToModelOnBattery(m_powerDBusProxy->batteryScreenBlackDelay());
    setSleepDelayToModelOnBattery(m_powerDBusProxy->batterySleepDelay());

    setResponseBatteryLockScreenDelay(m_powerDBusProxy->batteryLockDelay());
    setResponsePowerLockScreenDelay(m_powerDBusProxy->linePowerLockDelay());

    m_powerModel->setAutoPowerSaveMode(m_powerDBusProxy->powerSavingModeAuto());
    m_powerModel->setPowerSaveMode(m_powerDBusProxy->powerSavingModeEnabled());
    m_powerModel->setIsVirtualEnvironment(isVirtualEnvironment());

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const bool confVal = valueByQSettings<bool>(DCC_CONFIG_FILES, "Power", "sleep", true);
    const bool envVal = QVariant(env.value(POWER_CAN_SLEEP)).toBool();
    const bool envVal_hibernate = QVariant(env.value(POWER_CAN_HIBERNATE)).toBool();

    QFutureWatcher<bool> *canSleepWatcher = new QFutureWatcher<bool>();
    connect(canSleepWatcher, &QFutureWatcher<bool>::finished, this, [=] {
        bool canSuspend = canSleepWatcher->result();
        bool can_suspend = env.contains(POWER_CAN_SLEEP) ? envVal : confVal && canSuspend;
        m_powerModel->setCanSuspend(can_suspend);
        canSleepWatcher->deleteLater();
    });

    QFutureWatcher<bool> *canHibernateWatcher = new QFutureWatcher<bool>();
    connect(canHibernateWatcher, &QFutureWatcher<bool>::finished, this, [=] {
        bool canHibernate = canHibernateWatcher->result();
        bool can_hibernate = env.contains(POWER_CAN_HIBERNATE)
                                     ? envVal_hibernate
                                     : canHibernate;
        m_powerModel->setCanHibernate(can_hibernate);
        canHibernateWatcher->deleteLater();
    });

    canSleepWatcher->setFuture(QtConcurrent::run([=] {
        return m_powerDBusProxy->login1ManagerCanSuspend();
    }));

    canHibernateWatcher->setFuture(QtConcurrent::run([=] {
        return m_powerDBusProxy->login1ManagerCanHibernate();
    }));

    readConfig(BATTERY_LOCKDELAY_NAME, std::bind(&PowerModel::setBatteryLockDelayModel, m_powerModel, std::placeholders::_1));
    readConfig(BATTERY_SLEEPDELAY_NAME, std::bind(&PowerModel::setBatterySleepDelayModel, m_powerModel, std::placeholders::_1));
    readConfig(BATTERY_SBDELAY_NAME, std::bind(&PowerModel::setBatteryScreenBlackDelayModel, m_powerModel, std::placeholders::_1));
    readConfig(LINEPOWER_LOCKDELAY_NAME, std::bind(&PowerModel::setLinePowerLockDelayModel, m_powerModel, std::placeholders::_1));
    readConfig(LINEPOWER_SLEEPDELAY_NAME, std::bind(&PowerModel::setLinePowerSleepDelayModel, m_powerModel, std::placeholders::_1));
    readConfig(LINEPOWER_SBDELAY_NAME, std::bind(&PowerModel::setLinePowerScreenBlackDelayModel, m_powerModel, std::placeholders::_1));

    readConfig(SHOW_HIBERNATE_NAME, std::bind(&PowerModel::setHibernate, m_powerModel, std::placeholders::_1));
    readConfig(SHOW_SHUTDOWN_NAME, std::bind(&PowerModel::setShutdown, m_powerModel, std::placeholders::_1));
    readConfig(SHOW_SUSPEND_NAME, std::bind(&PowerModel::setSuspend, m_powerModel, std::placeholders::_1));
    readConfig(ENABLE_SCHEDULED_SHUTDOWN, std::bind(&PowerModel::setEnableScheduledShutdown, m_powerModel, std::placeholders::_1));

}

void PowerWorker::readConfig(const QString &key, std::function<void(const QVariantList &value)> callback)
{
    auto configList = m_cfgPower->value(key).toStringList();
    bool isLegal = true;
    if (configList.size() != 6) {
        isLegal = false;
    } else {
        QRegularExpression re("^[1-9][0-9]*[mh]");
        for (const QString &v : configList) {
            if (!re.match(v).hasMatch()) {
                isLegal = false;
                break;
            }
        }
    }
    if (!isLegal) {
        m_cfgPower->reset(key);
        qWarning() << "config is illegal, reset config";
    }
    auto configDataMap = converToDataMap(configList);
    callback(configDataMap);
}

void PowerWorker::readConfig(const QString &key, std::function<void(const bool value)> callback)
{
    bool enable = m_cfgPower->value(key, true).toBool();
    callback(enable);
}

void PowerWorker::readConfig(const QString &key, std::function<void(const QString &value)> callback)
{
    const QString &enable = m_cfgPower->value(key, true).toString();
    callback(enable);
}

void PowerWorker::deactive()
{
    m_powerDBusProxy->blockSignals(true);
}

void PowerWorker::setScreenBlackLock(const bool lock)
{
    m_powerDBusProxy->setScreenBlackLock(lock);
}

void PowerWorker::setScheduledShutdownState(const bool state)
{
    m_powerDBusProxy->setScheduledShutdownState(state);
}

void PowerWorker::setShutdownTime(const QString &time)
{
    m_powerDBusProxy->setShutdownTime(time);
}

void PowerWorker::setShutdownRepetition(int repetition)
{
    m_powerDBusProxy->setShutdownRepetition(repetition);
}

void PowerWorker::setCustomShutdownWeekDays(const QString &weekdays)
{
    QByteArray value;
    
    for (const auto &num : weekdays.simplified().split(',')) {
        bool ok = false;
        int day = num.toInt(&ok);
        if (ok) {
            value.append(static_cast<char>(day));
        }
    }
    m_powerDBusProxy->setCustomShutdownWeekDays(value);
}

void PowerWorker::setSleepLock(const bool lock)
{
    m_powerDBusProxy->setSleepLock(lock);
}

void PowerWorker::setSleepOnLidOnPowerClosed(const bool sleep)
{
    m_powerDBusProxy->setLidClosedSleep(sleep);
}

void PowerWorker::setSleepDelayOnPower(const int delay)
{
    qDebug() << "m_powerDBusProxy->setLinePowerSleepDelay: " << delay;
    m_powerDBusProxy->setLinePowerSleepDelay(delay);
}

void PowerWorker::setSleepDelayOnBattery(const int delay)
{
    qDebug() << "m_powerDBusProxy->setBatterySleepDelay: " << delay;
    m_powerDBusProxy->setBatterySleepDelay(delay);
}

void PowerWorker::setScreenBlackDelayOnPower(const int delay)
{
    qDebug() << "m_powerDBusProxy->setLinePowerScreenBlackDelay: " << delay;
    m_powerDBusProxy->setLinePowerScreenBlackDelay(delay);
}

void PowerWorker::setScreenBlackDelayOnBattery(const int delay)
{
    qDebug() << "m_powerDBusProxy->setBatteryScreenBlackDelay: " << delay;
    m_powerDBusProxy->setBatteryScreenBlackDelay(delay);
}

void PowerWorker::setSleepDelayToModelOnPower(const int delay)
{
    m_powerModel->setSleepDelayOnPower(delay);
}

void PowerWorker::setScreenBlackDelayToModelOnPower(const int delay)
{
    m_powerModel->setScreenBlackDelayOnPower(delay);
}

void PowerWorker::setSleepDelayToModelOnBattery(const int delay)
{
    m_powerModel->setSleepDelayOnBattery(delay);
}

void PowerWorker::setResponseBatteryLockScreenDelay(const int delay)
{
    m_powerModel->setBatteryLockScreenDelay(delay);
}

void PowerWorker::setResponsePowerLockScreenDelay(const int delay)
{
    m_powerModel->setPowerLockScreenDelay(delay);
}

void PowerWorker::setHighPerformanceSupported(bool state)
{
    m_powerModel->setHighPerformanceSupported(state);
}

void PowerWorker::setBalancePerformanceSupported(bool state)
{
    m_powerModel->setBalancePerformanceSupported(state);
}

void PowerWorker::setPowerSavingModeAutoWhenQuantifyLow(bool bLowBatteryAutoIntoSaveEnergyMode)
{
    m_powerDBusProxy->setPowerSavingModeAutoWhenBatteryLow(bLowBatteryAutoIntoSaveEnergyMode);
}

void PowerWorker::setPowerSavingModeAuto(bool bAutoIntoSaveEnergyMode)
{
    m_powerDBusProxy->setPowerSavingModeAuto(bAutoIntoSaveEnergyMode);
}

void PowerWorker::setPowerSavingModeLowerBrightnessThreshold(uint dPowerSavingModeLowerBrightnessThreshold)
{
    m_powerDBusProxy->setPowerSavingModeBrightnessDropPercent(dPowerSavingModeLowerBrightnessThreshold);
}

void PowerWorker::setPowerSavingModeAutoBatteryPercentage(uint dPowerSavingModeBatteryPercentage)
{
    m_powerDBusProxy->setPowerSavingModeAutoBatteryPercent(dPowerSavingModeBatteryPercentage);
}

void PowerWorker::setLinePowerPressPowerBtnAction(int nLinePowerPressPowerBtnAction)
{
    m_powerDBusProxy->setLinePowerPressPowerBtnAction(nLinePowerPressPowerBtnAction);
}

void PowerWorker::setLinePowerLidClosedAction(int nLinePowerLidClosedAction)
{
    m_powerDBusProxy->setLinePowerLidClosedAction(nLinePowerLidClosedAction);
}

void PowerWorker::setBatteryPressPowerBtnAction(int nBatteryPressPowerBtnAction)
{
    m_powerDBusProxy->setBatteryPressPowerBtnAction(nBatteryPressPowerBtnAction);
}

void PowerWorker::setBatteryLidClosedAction(int nBatteryLidClosedAction)
{
    m_powerDBusProxy->setBatteryLidClosedAction(nBatteryLidClosedAction);
}

void PowerWorker::setLowPowerNotifyEnable(bool bLowPowerNotifyEnable)
{
    m_powerDBusProxy->setLowPowerNotifyEnable(bLowPowerNotifyEnable);
}

void PowerWorker::setLowPowerNotifyThreshold(int dLowPowerNotifyThreshold)
{
    m_powerDBusProxy->setLowPowerNotifyThreshold(dLowPowerNotifyThreshold);
}

void PowerWorker::setLowPowerAutoSleepThreshold(int dLowPowerAutoSleepThreshold)
{
    m_powerDBusProxy->setLowPowerAutoSleepThreshold(dLowPowerAutoSleepThreshold);
}
void PowerWorker::setLowPowerAction(int action)
{
    m_powerDBusProxy->setLowPowerAction(action);
}

/**
 * @brief PowerWorker::setPowerPlan
 * @param powerPlan
 * 设置性能模式的dbus接口
 */
void PowerWorker::setPowerPlan(const QString &powerPlan)
{
    m_powerDBusProxy->SetMode(powerPlan);
}

bool PowerWorker::getCurCanSuspend()
{
    return m_powerDBusProxy->CanSuspend();
}

bool PowerWorker::getCurCanHibernate()
{
    return m_powerDBusProxy->CanHibernate();
}

void PowerWorker::setScreenBlackDelayToModelOnBattery(const int delay)
{
    m_powerModel->setScreenBlackDelayOnBattery(delay);
}

void PowerWorker::setLockScreenDelayOnBattery(const int delay)
{
    qDebug() << "m_powerDBusProxy->setBatteryLockDelay: " << delay;
    m_powerDBusProxy->setBatteryLockDelay(delay);
}

void PowerWorker::setLockScreenDelayOnPower(const int delay)
{
    qDebug() << "m_powerDBusProxy->setLinePowerLockDelay: " << delay;
    m_powerDBusProxy->setLinePowerLockDelay(delay);
}

void PowerWorker::setEnablePowerSave(const bool isEnable)
{
    m_powerDBusProxy->setPowerSavingModeEnabled(isEnable);
}

int PowerWorker::getMaxBacklightBrightness()
{
    return m_powerDBusProxy->maxBacklightBrightness();
}

void PowerWorker::setShowBatteryTimeToFull(bool value)
{
    if (m_cfgDock) {
        m_cfgDock->setValue("showTimeToFull", value);
    }
}

void PowerWorker::onCustomShutdownWeekDaysChanged(const QByteArray &value)
{
    QVariantList valueList;
    for (auto c : value) {
        valueList.append(static_cast<int>(c));
    }
    m_powerModel->setCustomShutdownWeekDays(valueList);
}
