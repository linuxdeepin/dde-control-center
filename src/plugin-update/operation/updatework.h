// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef UPDATEWORK_H
#define UPDATEWORK_H

#include "common.h"
#include "updatedbusproxy.h"
#include "updatejobdbusproxy.h"
#include "updatemodel.h"

#include <QDBusInterface>
#include <QObject>

const QString UpdateManagerService = QStringLiteral("org.deepin.UpdateManager1");
const QString UpdateManagerPath = QStringLiteral("/org/deepin/UpdateManager1");
const QString UpdateManagerInterface = QStringLiteral("org.deepin.UpdateManager1");
const QString PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
const QString PropertiesChanged = QStringLiteral("PropertiesChanged");

class UpdateWork : public QObject
{
    Q_OBJECT

public:
    explicit UpdateWork(UpdateModel* model, QObject *parent = nullptr);

    Q_INVOKABLE void checkUpgrade();
    Q_INVOKABLE void onActionBtnClicked();
    Q_INVOKABLE void showUpdateCtl(bool isShow);
    Q_INVOKABLE void checkProcessStop();
    Q_INVOKABLE void setSmartMirror(bool enable);
    void setCheckUpdatesJob(const QString &jobPath);
    void createCheckUpdateJob(const QString &jobPath);
    void createDistUpgradeJob(const QString &jobPath);

signals:

private Q_SLOTS:
    void onPropertiesChanged(const QDBusMessage &message);
    void onProcess(QPair<QString, double> processValue);
    void onUpdateStatusChanged(const QString &updateStatus);
    void onCheckUpdateStatusChanged(const QString &value);
    void onDistUpgradeStatusChanged(const QString &value);
    void deleteJob(UpdateJobDBusProxy *dbusJob);

private:
    void initData();
    void initConnection();
    void onJobListChanged(const QList<QDBusObjectPath> &jobs);

    void upgrade();
    UpdateErrorType analyzeJobErrorMessage(QString jobDescription);

    UpdateStatus* parseUpdateStatus(const QString &updateStatus);

    UpdateModel* m_model;
    UpdateDBusProxy *m_updateInter;
    UpdateJobDBusProxy *m_checkUpdateJob;
    UpdateJobDBusProxy *m_distUpgradeJob;
    QDBusInterface* m_updateInterface;

    QString m_jobPath;
};
#endif // UPDATEWORK_H
