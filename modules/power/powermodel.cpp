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

void PowerModel::setAutoPowerSaveMode(bool autoPowerSavingMode)
{
    if (m_autoPowerSaveMode == autoPowerSavingMode) return;

    m_autoPowerSaveMode = autoPowerSavingMode;

    emit autoPowerSavingModeChanged(autoPowerSavingMode);
}

void PowerModel::setPowerSaveMode(bool powerSaveMode)
{
    if (m_powerSaveMode == powerSaveMode) return;

    m_powerSaveMode = powerSaveMode;

    emit powerSaveModeChanged(powerSaveMode);
}

void PowerModel::setHaveBettary(bool haveBettary)
{
    if (haveBettary == m_haveBettary) return;

    m_haveBettary = haveBettary;

    emit haveBettaryChanged(haveBettary);
}

void PowerModel::setSleepLock(bool sleepLock)
{
    if (sleepLock != m_sleepLock) {
        m_sleepLock = sleepLock;

        emit sleepLockChanged(sleepLock);
    }
}
