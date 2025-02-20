// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UPDATEMODEL_H
#define UPDATEMODEL_H

#include "updatestatus.h"

#include <QObject>

class UpdateModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool upgradable READ upgradable NOTIFY upgradableChanged FINAL)
    Q_PROPERTY(QString updateState READ getUpdateState NOTIFY updateStateChanged FINAL)
    Q_PROPERTY(QString updateStateTips READ getUpdateStateTips NOTIFY updateStateTipsChanged FINAL)
    Q_PROPERTY(bool showUpdateCtl READ showUpdateCtl  NOTIFY showUpdateCtlChanged FINAL)
    Q_PROPERTY(int checkUpdateState READ checkUpdateState NOTIFY checkUpdateStateChanged FINAL)
    Q_PROPERTY(bool checkProcessRunning READ checkProcessRunning NOTIFY checkProcessRunningChanged FINAL)
    Q_PROPERTY(QString checkUpdateStateTips READ checkUpdateStateTips NOTIFY checkUpdateStateTipsChanged FINAL)
    Q_PROPERTY(QString checkUpdateIcon READ checkUpdateIcon NOTIFY checkUpdateIconChanged FINAL)
    Q_PROPERTY(QString actionBtnText READ actionBtnText NOTIFY actionBtnTextChanged FINAL)
    Q_PROPERTY(double checkUpdateProgress READ checkUpdateProgress NOTIFY checkUpdateProgressChanged FINAL)
    Q_PROPERTY(double distUpgradeProgress READ distUpgradeProgress NOTIFY distUpgradeProgressChanged FINAL)
    Q_PROPERTY(int distUpgradeState READ distUpgradeState NOTIFY distUpgradeStateChanged FINAL)
    Q_PROPERTY(bool smartMirrorSwitch READ smartMirrorSwitch NOTIFY smartMirrorSwitchChanged FINAL)


public:
    enum updateState
    {
        invalied = -1,
        noUpdate,
        upgrading,
        failed,
        needReboot
    };

    enum CheckUpdateState
    {
        NoState = -1,
        checking,
        checkFailed,
        checked
    };

    enum UpdateJobState
    {
        job_init = -1,
        job_ready,
        job_running,
        job_failed,
        job_successd,
    };

    explicit UpdateModel(QObject *parent = nullptr);
    ~UpdateModel();

    bool upgradable() const;
    void setUpgradable(bool newUpgradable);
    void updateUpgradble();

    QString getUpdateState() const;
    void setUpdateState(const QString &newUpdateState);

    QString getUpdateStateTips() const;
    void setUpdateStateTips(const QString &newUpdateStateTips);

    bool showUpdateCtl() const;
    void setShowUpdateCtl(bool newShowUpdateCtl);

    int checkUpdateState() const;
    void setCheckUpdateState(int newCheckUpdateState);

    bool checkProcessRunning() const;
    void setCheckProcessRunning(bool newCheckProcessRunning);

    QString checkUpdateStateTips() const;
    void setCheckUpdateStateTips(const QString &newCheckUpdateStateTips);

    QString checkUpdateIcon() const;
    void setCheckUpdateIcon(const QString &newCheckUpdateIcon);

    void updateCheckUpdateData();

    QString lastCheckUpdateErrorMsg() const;
    void setLastCheckUpdateErrorMsg(const QString &newLastCheckUpdateErrorMsg);

    QString actionBtnText() const;
    void setActionBtnText(const QString &newActionBtnText);

    UpdateStatus* updateStatus() const;
    void setUpdateStatus(UpdateStatus* status);

    double checkUpdateProgress() const;

    double distUpgradeProgress() const;

    int distUpgradeState() const;
    void setDistUpgradeState(int newDistUpgradeState);
    void updateDistUpgraedUI();

    bool smartMirrorSwitch() const;
    void setSmartMirrorSwitch(bool newSmartMirrorSwitch);

    void clearUpdateStatus();

signals:
    void upgradableChanged();
    void updateStateChanged();
    void updateStateTipsChanged();
    void showUpdateCtlChanged();
    void checkUpdateStateChanged();
    void checkProcessRunningChanged();
    void checkUpdateStateTipsChanged();
    void checkUpdateIconChanged();

    void actionBtnTextChanged();
    void updateStatusChanged(const UpdateStatus* status);

    void checkUpdateProgressChanged();

    void distUpgradeProgressChanged();

    void distUpgradeStateChanged();


    void smartMirrorSwitchChanged();

public Q_SLOTS:
    void setCheckUpdateProgress(double newCheckUpdateProgress);
    void setDistUpgradeProgress(double newDistUpgradeProgress);

private:
    bool m_upgradable;
    QString m_updateState;
    QString m_updateStateTips;
    bool m_showUpdateCtl;
    int m_checkUpdateState;
    int m_distUpgradeState;
    QString m_checkUpdateStateTips;
    QString m_checkUpdateIcon;
    bool m_checkProcessRunning;
    QString m_lastCheckUpdateErrorMsg;

    QString m_actionBtnText;

    UpdateStatus* m_updateStatus = nullptr;

    bool m_smartMirrorSwitch;


    double m_checkUpdateProgress;
    double m_distUpgradeProgress;
};

#endif // UPDATEMODEL_H
