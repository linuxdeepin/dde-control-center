// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "commoninfomodel.h"

#include <QDebug>

using namespace DCC_NAMESPACE;

CommonInfoModel::CommonInfoModel(QObject *parent)
    : QObject(parent)
    , m_bootDelay(false)
    , m_themeEnabled(false)
    , m_updating(false)
    , m_joinUeProgram(false)
    , m_activation(false)
    , m_plymouthscale(0)
    , m_plymouththeme(QString())
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

void CommonInfoModel::setPlymouthScale(int scale)
{
    m_plymouthscale = scale;

    Q_EMIT plymouthScaleChanged(scale);
}

void CommonInfoModel::setPlymouthTheme(const QString &themeName)
{
    m_plymouththeme = themeName;

    Q_EMIT plymouthThemeChanged(themeName);
}
