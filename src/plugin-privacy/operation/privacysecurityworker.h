//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef PRIVACYSECURITYWORKER_H
#define PRIVACYSECURITYWORKER_H

#include <QObject>
#include "privacysecuritydbusproxy.h"
#include "privacysecuritymodel.h"

class PrivacySecurityWorker : public QObject
{
    Q_OBJECT
public:
    explicit PrivacySecurityWorker(PrivacySecurityModel *model, QObject *parent = nullptr);
    ~PrivacySecurityWorker();

    void activate();
    void deactivate();

public:
    void permissionInfoLoadFinished(const QString &perInfo);
    // 设置总开关
    void getPermissionEnable(const QString& permissionGroup, const QString& permissionId);
    void setPermissionEnable(const QString& permissionGroup, const QString& permissionId, bool enable);

    // 设置App属性
    void setPermissionInfo(const QString& appId, const QString& permissionGroup, const QString& permissionId, const QString& value);
    const QString getIconPath(const QString& appName);

private:
    void saveServiceApps(const QString &currentGroup, const QString &dameonDefineName, const QJsonArray& appInfoDate);

public Q_SLOTS:
    void refreshPermissionState(const QString& permissionGroup, const QString& permissionId, bool enable);
    void resetPermissionState(const QString& permissionGroup, const QString& permissionId);
    void resetPermissionInfo(const QString& permissionGroup, const QString& permissionId);

private:
    PrivacySecurityModel *m_model;
    PrivacySecurityDBusProxy *m_privacyDBusInter;

    // 记录AppsName 异步多线程获取 IconPath
    QStringList m_appsName;
};

#endif // PRIVACYSECURITYWORKER_H
