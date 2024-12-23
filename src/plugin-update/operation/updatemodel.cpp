// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
#include "updatemodel.h"

#include <qdebug.h>

UpdateModel::UpdateModel(QObject *parent)
    : QObject{ parent }
    , m_upgradable(false)
    , m_updateState("")
    , m_updateStateTips("")
    , m_showUpdateCtl(false)
    , m_checkUpdateState(CheckUpdateState::NoState)
    , m_checkProcessRunning(false)
    , m_checkUpdateStateTips("")
    , m_checkUpdateIcon("")
    , m_lastCheckUpdateErrorMsg("")
    , m_actionBtnText(tr("Installing Update"))
{
}

bool UpdateModel::upgradable() const
{
    return m_upgradable;
}

void UpdateModel::setUpgradable(bool newUpgradable)
{
    if (m_upgradable == newUpgradable)
        return;
    m_upgradable = newUpgradable;
    emit upgradableChanged();
}

QString UpdateModel::getUpdateState() const
{
    return m_updateState;
}

void UpdateModel::setUpdateState(const QString &newUpdateState)
{
    qDebug() << "setUpdateState: " << newUpdateState << m_updateState;
    if (m_updateState == newUpdateState)
        return;
    m_updateState = newUpdateState;
    emit updateStateChanged();

    if (newUpdateState == "idle" && m_upgradable) {
        setUpdateStateTips(tr("Updates Available"));
        setActionBtnText(tr("Installing Update"));
        return;
    }

    if (newUpdateState == "upgrading") {
        setUpdateStateTips(tr("Installing updates…"));
        return;
    }

    if (newUpdateState == "success" ) {
        setUpdateStateTips(tr("Update installed successfully"));
        setActionBtnText(tr("Restart Now"));
        return;
    }

    if (newUpdateState == "failed") {
        setUpdateStateTips(tr("Update download failed"));
        setActionBtnText(tr("retry"));
    }
}

QString UpdateModel::getUpdateStateTips() const
{
    return m_updateStateTips;
}

void UpdateModel::setUpdateStateTips(const QString &newUpdateStateTips)
{
    if (m_updateStateTips == newUpdateStateTips)
        return;
    m_updateStateTips = newUpdateStateTips;
    emit updateStateTipsChanged();
}

bool UpdateModel::showUpdateCtl() const
{
    return m_showUpdateCtl;
}

void UpdateModel::setShowUpdateCtl(bool newShowUpdateCtl)
{
    if (m_showUpdateCtl == newShowUpdateCtl)
        return;
    m_showUpdateCtl = newShowUpdateCtl;
    emit showUpdateCtlChanged();
}

int UpdateModel::checkUpdateState() const
{
    return m_checkUpdateState;
}

void UpdateModel::setCheckUpdateState(int newCheckUpdateState)
{
    if (m_checkUpdateState == newCheckUpdateState)
        return;
    m_checkUpdateState = newCheckUpdateState;
    updateCheckUpdateData();
    emit checkUpdateStateChanged();
}

bool UpdateModel::checkProcessRunning() const
{
    return m_checkProcessRunning;
}

void UpdateModel::setCheckProcessRunning(bool newCheckProcessRunning)
{
    if (m_checkProcessRunning == newCheckProcessRunning)
        return;
    m_checkProcessRunning = newCheckProcessRunning;
    emit checkProcessRunningChanged();
    updateCheckUpdateData();
    setShowUpdateCtl(m_upgradable && !newCheckProcessRunning);
}

QString UpdateModel::checkUpdateStateTips() const
{
    return m_checkUpdateStateTips;
}

void UpdateModel::setCheckUpdateStateTips(const QString &newCheckUpdateStateTips)
{
    if (m_checkUpdateStateTips == newCheckUpdateStateTips)
        return;
    m_checkUpdateStateTips = newCheckUpdateStateTips;
    emit checkUpdateStateTipsChanged();
}

QString UpdateModel::checkUpdateIcon() const
{
    return m_checkUpdateIcon;
}

void UpdateModel::setCheckUpdateIcon(const QString &newCheckUpdateIcon)
{
    if (m_checkUpdateIcon == newCheckUpdateIcon)
        return;
    m_checkUpdateIcon = newCheckUpdateIcon;
    emit checkUpdateIconChanged();
}

void UpdateModel::updateCheckUpdateData()
{
    qInfo() << "updateCheckUpdateData : state, upgrade, processRunging" << m_checkUpdateState << m_upgradable << m_checkProcessRunning;
    switch (m_checkUpdateState) {
    case CheckUpdateState::NoState:
        break;
    case CheckUpdateState::checking:
        if (checkProcessRunning()) {
            setCheckUpdateStateTips(tr("Checking for updates, please wait…"));
            setCheckUpdateIcon("qrc:/icons/deepin/builtin/icons/dcc_updating.png");
        }
        break;
    case CheckUpdateState::checked:
        if (!upgradable() && !checkProcessRunning()) {
            setCheckUpdateStateTips(tr("Your system is up to date"));
            setCheckUpdateIcon("qrc:/icons/deepin/builtin/icons/dcc_update_success.png");
        }
        break;
    case CheckUpdateState::checkFailed:
        if (!checkProcessRunning()) {
            setCheckUpdateStateTips(lastCheckUpdateErrorMsg());
            setCheckUpdateIcon("qrc:/icons/deepin/builtin/icons/check_update_failed.png");
        }
        break;
    }
}

QString UpdateModel::lastCheckUpdateErrorMsg() const
{
    return m_lastCheckUpdateErrorMsg;
}

void UpdateModel::setLastCheckUpdateErrorMsg(const QString &newLastCheckUpdateErrorMsg)
{
    m_lastCheckUpdateErrorMsg = newLastCheckUpdateErrorMsg;
}

QString UpdateModel::actionBtnText() const
{
    return m_actionBtnText;
}

void UpdateModel::setActionBtnText(const QString &newActionBtnText)
{
    if (m_actionBtnText == newActionBtnText)
        return;
    m_actionBtnText = newActionBtnText;
    emit actionBtnTextChanged();
}
