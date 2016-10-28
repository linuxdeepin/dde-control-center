#include "powermodel.h"

#include <QDebug>

PowerModel::PowerModel(QObject *parent)
    : QObject(parent)
{
}

void PowerModel::setScreenBlackLock(const bool lock)
{
    m_screenBlackLock = lock;

    emit screenBlackLockChanged(lock);
}
