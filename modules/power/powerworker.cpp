#include "powerworker.h"

static const char *SuspendAction = "dbus-send --print-reply --dest=com.deepin.SessionManager /com/deepin/SessionManager com.deepin.SessionManager.RequestSuspend";

PowerWorker::PowerWorker(PowerModel *model, QObject *parent)
    : QObject(parent),

      m_powerModel(model),

      m_powerInter(new PowerInter("com.deepin.daemon.Power", "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this))
{
    m_powerInter->setSync(false);

    connect(m_powerInter, &PowerInter::ScreenBlackLockChanged, m_powerModel, &PowerModel::setScreenBlackLock);
    connect(m_powerInter, &PowerInter::SleepLockChanged, m_powerModel, &PowerModel::setSleepLock);
    connect(m_powerInter, &PowerInter::LidIsPresentChanged, m_powerModel, &PowerModel::setLidPresent);
    connect(m_powerInter, &PowerInter::LidClosedActionChanged, this, &PowerWorker::lidClosedActionChanged);
    connect(m_powerInter, &PowerInter::LinePowerScreenBlackDelayChanged, m_powerModel, &PowerModel::setScreenBlackDelay);
    connect(m_powerInter, &PowerInter::LinePowerSleepDelayChanged, m_powerModel, &PowerModel::setSleepDelay);

    active();
}

void PowerWorker::active()
{
    m_powerInter->blockSignals(false);

    // refersh data
    m_powerModel->setScreenBlackLock(m_powerInter->screenBlackLock());
    m_powerModel->setSleepLock(m_powerInter->sleepLock());
    m_powerModel->setLidPresent(m_powerInter->lidIsPresent());
    m_powerModel->setSleepOnLidClose(isActionSuspend(m_powerInter->lidClosedAction()));

    m_powerModel->setScreenBlackDelay(m_powerInter->linePowerScreenBlackDelay());
    m_powerModel->setSleepDelay(m_powerInter->linePowerSleepDelay());
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

void PowerWorker::setSleepOnLidClosed(const bool sleep)
{
    m_powerInter->setLidClosedAction(sleep ? SuspendAction : "");
}

void PowerWorker::setSleepDelay(const int delay)
{
    m_powerInter->setLinePowerSleepDelay(delay);
    m_powerInter->setBatterySleepDelay(delay);
}

void PowerWorker::setScreenBlackDelay(const int delay)
{
    m_powerInter->setLinePowerScreenBlackDelay(delay);
    m_powerInter->setBatteryScreenBlackDelay(delay);
}

void PowerWorker::lidClosedActionChanged(const QString &value)
{
    m_powerModel->setSleepOnLidClose(isActionSuspend(value));
}

bool PowerWorker::isActionSuspend(const QString &action)
{
    return action.contains("RequestSuspend");
}
