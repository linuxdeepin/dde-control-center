/*
 * Copyright (C) 2011 ~ 2023 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

private:
    PrivacySecurityModel *m_model;
    PrivacySecurityDBusProxy *m_privacyDBusInter;

    // 记录AppsName 异步多线程获取 IconPath
    QStringList m_appsName;
};

#endif // PRIVACYSECURITYWORKER_H
