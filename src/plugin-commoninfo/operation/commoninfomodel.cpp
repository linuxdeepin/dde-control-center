/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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

#include "commoninfomodel.h"
#include <QDebug>

DCC_USE_NAMESPACE

CommonInfoModel::CommonInfoModel(QObject *parent)
    : QObject(parent)
    , m_bootDelay(false)
{

}

void CommonInfoModel::setEntryLists(const QStringList &list)
{
    if (list != m_entryLists) {
        m_entryLists = list;
        Q_EMIT entryListsChanged(list);
    }
}

void CommonInfoModel::setThemeEnabled(const bool enabled)
{
    if (m_themeEnabled != enabled) {
        m_themeEnabled = enabled;
        Q_EMIT themeEnabledChanged(m_themeEnabled);
    }
}

void CommonInfoModel::setShowGrubEditAuth(const bool enabled)
{
    m_isShowGrubEditAuth = enabled;
}

void CommonInfoModel::setGrubEditAuthEnabled(const bool enabled)
{
    if (m_grubEditAuthEnabled != enabled) {
        m_grubEditAuthEnabled = enabled;
        Q_EMIT grubEditAuthEnabledChanged(m_grubEditAuthEnabled);
    }
}

void CommonInfoModel::setDefaultEntry(const QString &entry)
{
    if (m_defaultEntry != entry) {
        m_defaultEntry = entry;
        Q_EMIT defaultEntryChanged(entry);
    }
}

void CommonInfoModel::setUpdating(bool updating)
{
    if (updating != m_updating) {
        m_updating = updating;
        Q_EMIT updatingChanged(updating);
    }
}

void CommonInfoModel::setUeProgram(const bool ueProgram)
{
    if (m_joinUeProgram != ueProgram) {
        m_joinUeProgram = ueProgram;
    }
    Q_EMIT ueProgramChanged(m_joinUeProgram);
}

void CommonInfoModel::setDeveloperModeState(const bool state)
{
    if (m_developerModeState != state) {
        m_developerModeState = state;
        Q_EMIT developerModeStateChanged(state);
    }
}

void CommonInfoModel::setIsLogin(const bool log)
{
    if (m_isLogin == log)
        return;

    m_isLogin = log;
    Q_EMIT isLoginChenged(log);
}

bool CommonInfoModel::bootDelay() const
{
    return m_bootDelay;
}

void CommonInfoModel::setBootDelay(bool bootDelay)
{
    qDebug()<<" CommonInfoModel::setBootDelay   bootDelay =  "<< bootDelay;
    if (m_bootDelay != bootDelay) {
        m_bootDelay = bootDelay;
        Q_EMIT bootDelayChanged(bootDelay);
    }
}

void CommonInfoModel::setActivation(bool value)
{
    if (m_activation != value) {
        m_activation = value;
        Q_EMIT LicenseStateChanged(value);
    }
}


QPixmap CommonInfoModel::background() const
{
    return m_background;
}

void CommonInfoModel::setBackground(const QPixmap &background)
{
    m_background = background;

    Q_EMIT backgroundChanged(background);
}

bool CommonInfoModel::ueProgram() const
{
    return m_joinUeProgram;
}

bool CommonInfoModel::developerModeState() const
{
    return m_developerModeState;
}
