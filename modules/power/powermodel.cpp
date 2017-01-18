#include "powermodel.h"

#include <QDebug>

using namespace dcc;
using namespace dcc::power;

PowerModel::PowerModel(QObject *parent)
    : QObject(parent)
{
}

void PowerModel::setScreenBlackLock(const bool lock)
{
    if (lock != m_screenBlackLock) {
        m_screenBlackLock = lock;

        emit screenBlackLockChanged(lock);
    }
}

void PowerModel::setLidPresent(bool lidPresent)
{
    if (lidPresent != m_lidPresent) {
        m_lidPresent = lidPresent;

        emit lidPresentChanged(lidPresent);
    }
}

void PowerModel::setScreenBlackDelay(const int screenBlackDelay)
{
    if (screenBlackDelay != m_screenBlackDelay) {
        m_screenBlackDelay = screenBlackDelay;

        emit screenBlackDelayChanged(screenBlackDelay);
    }
}

void PowerModel::setSleepDelay(const int sleepDelay)
{
    if (sleepDelay != m_sleepDelay) {
        m_sleepDelay = sleepDelay;

        emit sleepDelayChanged(sleepDelay);
    }
}

void PowerModel::setSleepOnLidClose(bool sleepOnLidClose)
{
    if (sleepOnLidClose != m_sleepOnLidClose) {
        m_sleepOnLidClose = sleepOnLidClose;

        emit sleepOnLidCloseChanged(sleepOnLidClose);
    }
}

void PowerModel::setSleepLock(bool sleepLock)
{
    if (sleepLock != m_sleepLock) {
        m_sleepLock = sleepLock;

        emit sleepLockChanged(sleepLock);
    }
}
