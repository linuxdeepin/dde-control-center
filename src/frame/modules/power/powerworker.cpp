// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "powerworker.h"
#include "powermodel.h"
#include "widgets/utils.h"

#include <QProcessEnvironment>
#include <QFutureWatcher>
#include <QtConcurrent>

#define POWER_CAN_SLEEP "POWER_CAN_SLEEP"
#define POWER_CAN_HIBERNATE "POWER_CAN_HIBERNATE"

using namespace dcc;
using namespace dcc::power;

PowerWorker::PowerWorker(PowerModel *model, QObject *parent)
    : QObject(parent)
    , m_powerModel(model)
    , m_powerInter(new PowerInter("com.deepin.daemon.Power", "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this))
    , m_sysPowerInter(new SysPowerInter("com.deepin.system.Power", "/com/deepin/system/Power", QDBusConnection::systemBus(), this))
    , m_login1ManagerInter(new Login1ManagerInter("org.freedesktop.login1", "/org/freedesktop/login1", QDBusConnection::systemBus(), this))
    , m_powerManager(new PowerManager("com.deepin.daemon.PowerManager", "/com/deepin/daemon/PowerManager", QDBusConnection::systemBus(), this))
{
    m_powerInter->setSync(false);
    m_sysPowerInter->setSync(false);
    m_login1ManagerInter->setSync(false);

    connect(m_powerInter, &PowerInter::ScreenBlackLockChanged, m_powerModel, &PowerModel::setScreenBlackLock);
    connect(m_powerInter, &PowerInter::SleepLockChanged, m_powerModel, &PowerModel::setSleepLock);
    connect(m_powerInter, &PowerInter::LidIsPresentChanged, m_powerModel, &PowerModel::setLidPresent);
    connect(m_powerInter, &PowerInter::LidClosedSleepChanged, m_powerModel, &PowerModel::setSleepOnLidOnPowerClose);
//    connect(m_powerInter, &PowerInter::BatteryLidClosedSleepChanged, m_powerModel, &PowerModel::setSleepOnLidOnBatteryClose);
    connect(m_powerInter, &PowerInter::LinePowerScreenBlackDelayChanged, this, &PowerWorker::setScreenBlackDelayToModelOnPower);
    // 异步获取初始化时获取到的是0，这里在收到变化信号后设置一次
    connect(m_powerInter, &PowerInter::LinePowerSleepDelayChanged, this, [this] (int value) {
        setSleepDelayToModelOnPower(value);
        const QStringList& conf = m_powerModel->getLinePowerSleepDelayConf();
        if (value != converToDelayDBus(conf, converToDelayModel(conf, value))) {
            setSleepDelayOnPower(converToDelayModel(conf, value));
        }
    });
    connect(m_powerInter, &PowerInter::BatteryScreenBlackDelayChanged, this, &PowerWorker::setScreenBlackDelayToModelOnBattery);
    connect(m_powerInter, &PowerInter::BatterySleepDelayChanged, this, [this] (int value) {
        setSleepDelayToModelOnBattery(value);
        const QStringList& conf = m_powerModel->getBatterySleepDelayConf();
        if (value != converToDelayDBus(conf, converToDelayModel(conf, value))) {
            setSleepDelayOnBattery(converToDelayModel(conf, value));
        }
    });
    connect(m_powerInter, &PowerInter::BatteryLockDelayChanged, this, &PowerWorker::setResponseBatteryLockScreenDelay);
    connect(m_powerInter, &PowerInter::LinePowerLockDelayChanged, this, &PowerWorker::setResponsePowerLockScreenDelay);
    connect(m_powerInter, &PowerInter::IsHighPerformanceSupportedChanged, this, &PowerWorker::setHighPerformanceSupported);

#ifndef DCC_DISABLE_POWERSAVE
    connect(m_sysPowerInter, &SysPowerInter::PowerSavingModeAutoChanged, m_powerModel, &PowerModel::setAutoPowerSaveMode);
    connect(m_sysPowerInter, &SysPowerInter::PowerSavingModeEnabledChanged, m_powerModel, &PowerModel::setPowerSaveMode);
#endif
    connect(m_sysPowerInter, &SysPowerInter::HasBatteryChanged, m_powerModel, &PowerModel::setHaveBettary);
    connect(m_sysPowerInter, &SysPowerInter::BatteryPercentageChanged, m_powerModel, &PowerModel::setBatteryPercentage);


    //--------------------sp2 add----------------------------
    connect(m_sysPowerInter, &SysPowerInter::PowerSavingModeAutoWhenBatteryLowChanged, m_powerModel, &PowerModel::setPowerSavingModeAutoWhenQuantifyLow);
    connect(m_sysPowerInter, &SysPowerInter::PowerSavingModeAutoChanged, m_powerModel, &PowerModel::setPowerSavingModeAuto);
    connect(m_sysPowerInter, &SysPowerInter::PowerSavingModeBrightnessDropPercentChanged, m_powerModel, &PowerModel::setPowerSavingModeLowerBrightnessThreshold);
    connect(m_powerInter, &PowerInter::LinePowerPressPowerBtnActionChanged, m_powerModel, &PowerModel::setLinePowerPressPowerBtnAction);
    connect(m_powerInter, &PowerInter::LinePowerLidClosedActionChanged, m_powerModel, &PowerModel::setLinePowerLidClosedAction);
    connect(m_powerInter, &PowerInter::BatteryPressPowerBtnActionChanged, m_powerModel, &PowerModel::setBatteryPressPowerBtnAction);
    connect(m_powerInter, &PowerInter::BatteryLidClosedActionChanged, m_powerModel, &PowerModel::setBatteryLidClosedAction);
    connect(m_powerInter, &PowerInter::LowPowerNotifyEnableChanged, m_powerModel, &PowerModel::setLowPowerNotifyEnable);
    connect(m_powerInter, &PowerInter::LowPowerNotifyThresholdChanged, m_powerModel, &PowerModel::setLowPowerNotifyThreshold);
    connect(m_powerInter, &PowerInter::LowPowerAutoSleepThresholdChanged, m_powerModel, &PowerModel::setLowPowerAutoSleepThreshold);
    //-------------------------------------------------------
    connect(m_sysPowerInter, &SysPowerInter::ModeChanged, m_powerModel, &PowerModel::setPowerPlan);

    bool value = false;
    QDBusInterface interface("com.deepin.system.Power",
                             "/com/deepin/system/Power",
                             "org.freedesktop.DBus.Properties",
                             QDBusConnection::systemBus());
    QDBusMessage reply = interface.call("Get", "com.deepin.system.Power", "IsBalanceSupported");
    QList<QVariant> outArgs = reply.arguments();
    if (outArgs.length() > 0) {
        value = outArgs.at(0).value<QDBusVariant>().variant().toBool();
        m_powerModel->setBalanceSupported(value);
    }

    reply = interface.call("Get", "com.deepin.system.Power", "IsPowerSaveSupported");
    outArgs = reply.arguments();
    if (outArgs.length() > 0) {
        value = outArgs.at(0).value<QDBusVariant>().variant().toBool();
        m_powerModel->setPowerSaveSupported(value);
    }
}

void PowerWorker::active()
{
    m_powerInter->blockSignals(false);

    // refersh data
    m_powerModel->setScreenBlackLock(m_powerInter->screenBlackLock());
    m_powerModel->setSleepLock(m_powerInter->sleepLock());
    m_powerModel->setLidPresent(m_powerInter->lidIsPresent());
    m_powerModel->setSleepOnLidOnPowerClose(m_powerInter->lidClosedSleep());
    m_powerModel->setHaveBettary(m_sysPowerInter->hasBattery());
    m_powerModel->setPowerSavingModeAutoWhenQuantifyLow(m_sysPowerInter->powerSavingModeAutoWhenBatteryLow());
    m_powerModel->setPowerSavingModeLowerBrightnessThreshold(m_sysPowerInter->powerSavingModeBrightnessDropPercent());
    m_powerModel->setLowPowerNotifyEnable(m_powerInter->lowPowerNotifyEnable());
    m_powerModel->setLowPowerAutoSleepThreshold(m_powerInter->lowPowerAutoSleepThreshold());
    m_powerModel->setLowPowerNotifyThreshold(m_powerInter->lowPowerNotifyThreshold());
    m_powerModel->setLinePowerPressPowerBtnAction(m_powerInter->linePowerPressPowerBtnAction());
    m_powerModel->setLinePowerLidClosedAction(m_powerInter->linePowerLidClosedAction());
    m_powerModel->setBatteryPressPowerBtnAction(m_powerInter->batteryPressPowerBtnAction());
    m_powerModel->setBatteryLidClosedAction(m_powerInter->batteryLidClosedAction());
    m_powerModel->setPowerPlan(m_sysPowerInter->mode());

    setHighPerformanceSupported(m_powerInter->isHighPerformanceSupported());

    setScreenBlackDelayToModelOnPower(m_powerInter->linePowerScreenBlackDelay());
    setSleepDelayToModelOnPower(m_powerInter->linePowerSleepDelay());

    setScreenBlackDelayToModelOnBattery(m_powerInter->batteryScreenBlackDelay());
    setSleepDelayToModelOnBattery(m_powerInter->batterySleepDelay());

    setResponseBatteryLockScreenDelay(m_powerInter->batteryLockDelay());
    setResponsePowerLockScreenDelay(m_powerInter->linePowerLockDelay());

#ifndef DCC_DISABLE_POWERSAVE
    m_powerModel->setAutoPowerSaveMode(m_sysPowerInter->powerSavingModeAuto());
    m_powerModel->setPowerSaveMode(m_sysPowerInter->powerSavingModeEnabled());
#endif

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    const bool confVal = valueByQSettings<bool>(DCC_CONFIG_FILES, "Power", "sleep", true);
    const bool envVal = QVariant(env.value(POWER_CAN_SLEEP)).toBool();
    const bool envVal_hibernate  = QVariant(env.value(POWER_CAN_HIBERNATE)).toBool();

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

    // 使用PowerManager接口，此接口中有是否虚拟机判断
    canSleepWatcher->setFuture(QtConcurrent::run([=] {
        bool reply = m_powerManager->CanSuspend();
        return reply;
    }));

    // 使用PowerManager接口，此接口中有是否虚拟机判断
    canHibernateWatcher->setFuture(QtConcurrent::run([=] {
        bool reply = m_powerManager->CanHibernate();
        return reply;
    }));
}

void PowerWorker::deactive()
{
    m_powerInter->blockSignals(true);
}

void PowerWorker::setScreenBlackLock(const bool lock)
{
    m_powerInter->setScreenBlackLock(lock);
}

void PowerWorker::setSleepLock(const bool lock)
{
    m_powerInter->setSleepLock(lock);
}

void PowerWorker::setSleepOnLidOnPowerClosed(const bool sleep)
{
    m_powerInter->setLidClosedSleep(sleep);
}

void PowerWorker::setSleepDelayOnPower(const int delay)
{
    qDebug() << "m_powerInter->setLinePowerSleepDelay: " << converToDelayDBus(m_powerModel->getLinePowerSleepDelayConf(), delay);
    m_powerInter->setLinePowerSleepDelay(converToDelayDBus(m_powerModel->getLinePowerSleepDelayConf(), delay));
}

void PowerWorker::setSleepDelayOnBattery(const int delay)
{
    qDebug() << "m_powerInter->setBatterySleepDelay: " << converToDelayDBus(m_powerModel->getBatterySleepDelayConf(), delay);
    m_powerInter->setBatterySleepDelay(converToDelayDBus(m_powerModel->getBatterySleepDelayConf(), delay));
}

void PowerWorker::setScreenBlackDelayOnPower(const int delay)
{
    qDebug() << "m_powerInter->setLinePowerScreenBlackDelay: " << converToDelayDBus(m_powerModel->getLinePowerScreenBlackDelayConf(), delay);
    m_powerInter->setLinePowerScreenBlackDelay(converToDelayDBus(m_powerModel->getLinePowerScreenBlackDelayConf(), delay));
}

void PowerWorker::setScreenBlackDelayOnBattery(const int delay)
{
    qDebug() << "m_powerInter->setBatteryScreenBlackDelay: " << converToDelayDBus(m_powerModel->getBatteryScreenBlackDelayConf(), delay);
    m_powerInter->setBatteryScreenBlackDelay(converToDelayDBus(m_powerModel->getBatteryScreenBlackDelayConf(), delay));
}

void PowerWorker::setSleepDelayToModelOnPower(const int delay)
{
    qDebug() << "m_powerModel->setLinePowerSleepDelay: " << converToDelayModel(m_powerModel->getLinePowerSleepDelayConf(), delay);
    m_powerModel->setSleepDelayOnPower(converToDelayModel(m_powerModel->getLinePowerSleepDelayConf(), delay));
}

void PowerWorker::setScreenBlackDelayToModelOnPower(const int delay)
{
    m_powerModel->setScreenBlackDelayOnPower(converToDelayModel(m_powerModel->getLinePowerScreenBlackDelayConf(), delay));
}

void PowerWorker::setSleepDelayToModelOnBattery(const int delay)
{
    m_powerModel->setSleepDelayOnBattery(converToDelayModel(m_powerModel->getBatterySleepDelayConf(), delay));
}

void PowerWorker::setResponseBatteryLockScreenDelay(const int delay)
{
    m_powerModel->setBatteryLockScreenDelay(converToDelayModel(m_powerModel->getBatteryLockDelayConf(), delay));
}

void PowerWorker::setResponsePowerLockScreenDelay(const int delay)
{
    m_powerModel->setPowerLockScreenDelay(converToDelayModel(m_powerModel->getLinePowerLockDelayConf(), delay));
}

void PowerWorker::setHighPerformanceSupported(bool state)
{
    m_powerModel->setHighPerformanceSupported(state);
}

void PowerWorker::setPowerSavingModeAutoWhenQuantifyLow(bool bLowBatteryAutoIntoSaveEnergyMode)
{
    m_sysPowerInter->setPowerSavingModeAutoWhenBatteryLow(bLowBatteryAutoIntoSaveEnergyMode);
}

void PowerWorker::setPowerSavingModeAuto(bool bAutoIntoSaveEnergyMode)
{
    m_sysPowerInter->setPowerSavingModeAuto(bAutoIntoSaveEnergyMode);
}

void PowerWorker::setPowerSavingModeLowerBrightnessThreshold(uint dPowerSavingModeLowerBrightnessThreshold)
{
    m_sysPowerInter->setPowerSavingModeBrightnessDropPercent(dPowerSavingModeLowerBrightnessThreshold);
}

void PowerWorker::setLinePowerPressPowerBtnAction(int nLinePowerPressPowerBtnAction)
{
    m_powerInter->setLinePowerPressPowerBtnAction(nLinePowerPressPowerBtnAction);
}

void PowerWorker::setLinePowerLidClosedAction(int nLinePowerLidClosedAction)
{
    m_powerInter->setLinePowerLidClosedAction(nLinePowerLidClosedAction);
}

void PowerWorker::setBatteryPressPowerBtnAction(int nBatteryPressPowerBtnAction)
{
    m_powerInter->setBatteryPressPowerBtnAction(nBatteryPressPowerBtnAction);
}

void PowerWorker::setBatteryLidClosedAction(int nBatteryLidClosedAction)
{
    m_powerInter->setBatteryLidClosedAction(nBatteryLidClosedAction);
}

void PowerWorker::setLowPowerNotifyEnable(bool bLowPowerNotifyEnable)
{
    m_powerInter->setLowPowerNotifyEnable(bLowPowerNotifyEnable);
}

void PowerWorker::setLowPowerNotifyThreshold(int dLowPowerNotifyThreshold)
{
    m_powerInter->setLowPowerNotifyThreshold(dLowPowerNotifyThreshold);
}

void PowerWorker::setLowPowerAutoSleepThreshold(int dLowPowerAutoSleepThreshold)
{
    m_powerInter->setLowPowerAutoSleepThreshold(dLowPowerAutoSleepThreshold);
}

/**
 * @brief PowerWorker::setPowerPlan
 * @param powerPlan
 * 设置性能模式的dbus接口
 */
void PowerWorker::setPowerPlan(const QString &powerPlan)
{
    m_sysPowerInter->SetMode(powerPlan);
}

bool PowerWorker::getCurCanSuspend()
{
    return m_powerManager->CanSuspend();
}

bool PowerWorker::getCurCanHibernate()
{
    return m_powerManager->CanHibernate();
}

void PowerWorker::setScreenBlackDelayToModelOnBattery(const int delay)
{
    m_powerModel->setScreenBlackDelayOnBattery(converToDelayModel(m_powerModel->getBatteryScreenBlackDelayConf(), delay));
}

void PowerWorker::setLockScreenDelayOnBattery(const int delay)
{
    qDebug() << "m_powerInter->setBatteryLockDelay: " << converToDelayDBus(m_powerModel->getBatteryLockDelayConf(), delay);
    m_powerInter->setBatteryLockDelay(converToDelayDBus(m_powerModel->getBatteryLockDelayConf(), delay));
}

void PowerWorker::setLockScreenDelayOnPower(const int delay)
{
    qDebug() << "m_powerInter->setLinePowerLockDelay: " << converToDelayDBus(m_powerModel->getLinePowerLockDelayConf(), delay);
    m_powerInter->setLinePowerLockDelay(converToDelayDBus(m_powerModel->getLinePowerLockDelayConf(), delay));
}

#ifndef DCC_DISABLE_POWERSAVE
void PowerWorker::setEnablePowerSave(const bool isEnable)
{
    m_sysPowerInter->setPowerSavingModeEnabled(isEnable);
}

void PowerWorker::setAutoEnablePowerSave(const bool isEnable)
{
    m_sysPowerInter->setPowerSavingModeAuto(isEnable);
}
#endif

QVector<int> PowerWorker::converToNum(const QStringList& conf) const
{
    QVector<int> numConf;
    for(const QString& numStr : conf) {
        int num;
        if (numStr.isEmpty()) {
            qWarning() << "config is empty!";
            num = 0;
        }
        bool ok;
        num = numStr.mid(0, numStr.length() - 1).toInt(&ok);
        if (!ok) {
            qWarning() << "change to int failed!";
            num = 0;
        }
        if (numStr.contains("m")) {
            num = num * 60;
        } else if (numStr.contains("h")) {
            num = num * 3600;
        }
        numConf.push_back(num);
    }
    return numConf;
}

int PowerWorker::converToDelayModel(const QStringList& conf, int value)
{
    if (value == 0 || conf.size() != 6) {
        return 7;
    }
    QVector<int> numConf = converToNum(conf);
    if (value <= numConf[0]) {
        return 1;
    } else if (value <= numConf[1]) {
        return 2;
    } else if (value <= numConf[2]) {
        return 3;
    } else if (value <= numConf[3]) {
        return 4;
    } else if (value <= numConf[4]) {
        return 5;
    } else {
        return 6;
    }
}

int PowerWorker::converToDelayDBus(const QStringList& conf, int value)
{
    if (value >= 7 || conf.size() != 6) {
        return 0;
    }
    return converToNum(conf)[value - 1];
}
