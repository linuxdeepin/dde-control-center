// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
#include "updatemodel.h"

#include <qdebug.h>

UpdateModel::UpdateModel(QObject *parent)
    : QObject(parent),
      m_upgradable(false),
      m_updateState(""),
      m_updateStateTips(""),
      m_showUpdateCtl(false),
      m_checkUpdateState(CheckUpdateState::NoState),
      m_distUpgradeState(job_init),
      m_checkUpdateStateTips(""),
      m_checkUpdateIcon(""),
      m_checkProcessRunning(false),
      m_lastCheckUpdateErrorMsg(""),
      m_actionBtnText(""),
      m_updateStatus(nullptr)
{
}

UpdateModel::~UpdateModel()
{
    delete m_updateStatus;
}

bool UpdateModel::upgradable() const
{
    return m_upgradable;
}

void UpdateModel::setUpgradable(bool newUpgradable)
{
    qDebug() << "UpdateModel setUpgradable" << newUpgradable << m_upgradable;
    if (m_upgradable == newUpgradable)
        return;
    m_upgradable = newUpgradable;
    emit upgradableChanged();
}

void UpdateModel::updateUpgradble()
{
    UpdateStatusData* status = m_updateStatus->statusData();
    if (!status) {
        setUpgradable(false);
        return;
    }

    qDebug() << " updateUpgradble status:" << status->securityUpgrade() << status->systemUpgrade() << status->unknowUpgrade() ;

    auto hasUpdate = [](QString state)->bool {
        return state != "noUpdate" && state != "needReboot";
    };

    setUpgradable(hasUpdate(status->systemUpgrade()) || hasUpdate(status->securityUpgrade()) || hasUpdate(status->unknowUpgrade()));
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

    if (newUpdateState == "noUpdate" && m_upgradable) {
        setUpdateStateTips(tr("Updates Available"));
        setActionBtnText(tr("Installing Update"));
        return;
    }

    if (newUpdateState == "upgrading") {
        setUpdateStateTips(tr("Installing updates…"));
        return;
    }

    if (newUpdateState == "needReboot" ) {
        setUpdateStateTips(tr("Update installed successfully"));
        setActionBtnText(tr("Restart Now"));
        return;
    }

    if (newUpdateState == "upgradeFailed") {
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
    updateCheckUpdateData();
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
        setCheckUpdateStateTips(tr("Checking for updates, please wait…"));
        setCheckUpdateIcon("updating");
        break;
    case CheckUpdateState::checked:
        if (!upgradable()) {
            UpdateStatusData* status = m_updateStatus->statusData();
            if (status->systemUpgrade() == "needReboot" || status->securityUpgrade() == "needReboot" || status->unknowUpgrade() == "needReboot") {
                setCheckUpdateStateTips(tr("Your system is up to date, please restart now"));
            } else {
                setCheckUpdateStateTips(tr("Your system is up to date"));
            }

            setCheckUpdateIcon("update_abreast_of_time");
        }
        break;
    case CheckUpdateState::checkFailed:
        setCheckUpdateStateTips(lastCheckUpdateErrorMsg());
        setCheckUpdateIcon("update_failure");
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

UpdateStatus* UpdateModel::updateStatus() const
{
    return m_updateStatus;
}

void UpdateModel::setUpdateStatus(UpdateStatus* status)
{
    if (m_updateStatus != status) {
        delete m_updateStatus;
        m_updateStatus = status;
        updateUpgradble();
        emit updateStatusChanged(m_updateStatus);
    }
}

void UpdateModel::clearUpdateStatus()
{
    delete m_updateStatus;
    m_updateStatus = nullptr;
}

double UpdateModel::checkUpdateProgress() const
{
    return m_checkUpdateProgress;
}

void UpdateModel::setCheckUpdateProgress(double newCheckUpdateProgress)
{
    if (qFuzzyCompare(m_checkUpdateProgress, newCheckUpdateProgress))
        return;
    m_checkUpdateProgress = newCheckUpdateProgress;
    emit checkUpdateProgressChanged();
}

double UpdateModel::distUpgradeProgress() const
{
    return m_distUpgradeProgress;
}

void UpdateModel::setDistUpgradeProgress(double newDistUpgradeProgress)
{
    newDistUpgradeProgress = newDistUpgradeProgress * 100.00;
    if (qFuzzyCompare(m_distUpgradeProgress, newDistUpgradeProgress))
        return;
    m_distUpgradeProgress = newDistUpgradeProgress;
    emit distUpgradeProgressChanged();
}

bool UpdateModel::smartMirrorSwitch() const
{
    return m_smartMirrorSwitch;
}

void UpdateModel::setSmartMirrorSwitch(bool newSmartMirrorSwitch)
{
    if (m_smartMirrorSwitch == newSmartMirrorSwitch)
        return;
    m_smartMirrorSwitch = newSmartMirrorSwitch;
    emit smartMirrorSwitchChanged();
}

int UpdateModel::distUpgradeState() const
{
    return m_distUpgradeState;
}

void UpdateModel::setDistUpgradeState(int newDistUpgradeState)
{
    if (m_distUpgradeState == newDistUpgradeState)
        return;
    m_distUpgradeState = newDistUpgradeState;
    updateDistUpgraedUI();
    emit distUpgradeStateChanged();
}

void UpdateModel::updateDistUpgraedUI()
{
    switch (m_distUpgradeState) {
        case job_init:
            setUpdateStateTips(tr("An update has been detected."));
            setActionBtnText(tr("Installing Update"));
            break;
        case job_ready :
        case job_running:
            setUpdateStateTips(tr("Installing updates…"));
            break;
        case job_failed:
            setUpdateStateTips(tr("Update download failed"));
            setActionBtnText(tr("retry"));
            break;
        case job_successd:
            setUpdateStateTips(tr("Update installed successfully"));
            setActionBtnText(tr("Restart Now"));
            break;
    }
}


