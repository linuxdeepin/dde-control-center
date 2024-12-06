// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UPDATEMODEL_H
#define UPDATEMODEL_H

#include <QObject>

class UpdateModel : public QObject
{
    Q_OBJECT

public:
    enum updateState
    {
        invalied = -1,
        idle,
        upgrading,
        failed
    };

    enum CheckUpdateState
    {
        NoState = -1,
        checking,
        checkFailed,
        checked
    };

    explicit UpdateModel(QObject *parent = nullptr);

    bool upgradable() const;
    void setUpgradable(bool newUpgradable);

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

private:
    bool m_upgradable;
    QString m_updateState;
    QString m_updateStateTips;
    bool m_showUpdateCtl;
    int m_checkUpdateState;
    QString m_checkUpdateStateTips;
    QString m_checkUpdateIcon;
    bool m_checkProcessRunning;
    QString m_lastCheckUpdateErrorMsg;

    QString m_actionBtnText;

    Q_PROPERTY(bool upgradable READ upgradable NOTIFY upgradableChanged FINAL)
    Q_PROPERTY(QString updateState READ getUpdateState NOTIFY updateStateChanged FINAL)
    Q_PROPERTY(QString updateStateTips READ getUpdateStateTips NOTIFY updateStateTipsChanged FINAL)
    Q_PROPERTY(bool showUpdateCtl READ showUpdateCtl  NOTIFY showUpdateCtlChanged FINAL)
    Q_PROPERTY(int checkUpdateState READ checkUpdateState NOTIFY checkUpdateStateChanged FINAL)
    Q_PROPERTY(bool checkProcessRunning READ checkProcessRunning NOTIFY checkProcessRunningChanged FINAL)
    Q_PROPERTY(QString checkUpdateStateTips READ checkUpdateStateTips NOTIFY checkUpdateStateTipsChanged FINAL)
    Q_PROPERTY(QString checkUpdateIcon READ checkUpdateIcon NOTIFY checkUpdateIconChanged FINAL)
    Q_PROPERTY(QString actionBtnText READ actionBtnText NOTIFY actionBtnTextChanged FINAL)
};

#endif // UPDATEMODEL_H
