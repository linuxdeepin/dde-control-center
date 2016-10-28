#include "powerworker.h"

PowerWorker::PowerWorker(PowerModel *model, QObject *parent)
    : QObject(parent),

      m_powerModel(model),

      m_powerInter(new PowerInter("com.deepin.daemon.Power", "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this))
{
    m_powerInter->setSync(false);

    connect(m_powerInter, &PowerInter::ScreenBlackLockChanged, m_powerModel, &PowerModel::setScreenBlackLock);

    active();
}

void PowerWorker::active()
{
    m_powerInter->blockSignals(false);

    // refersh data
    m_powerModel->setScreenBlackLock(m_powerInter->screenBlackLock());
}

void PowerWorker::deactive()
{
    m_powerInter->blockSignals(true);
}

void PowerWorker::setScreenBlackLock(const bool lock)
{
    m_powerInter->setScreenBlackLock(lock);
}
