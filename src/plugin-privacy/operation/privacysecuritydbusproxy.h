//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef PRIVACYSECURITYDBUSPROXY_H
#define PRIVACYSECURITYDBUSPROXY_H

#include <QObject>
#include "interface/namespace.h"

#include <DDBusInterface>

using Dtk::Core::DDBusInterface;


class PrivacySecurityDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit PrivacySecurityDBusProxy(QObject *parent = nullptr);

    void getPermissionInfo();

    // 设置权限信息
    void setPermissionInfo(const QString& appId, const QString& permissionGroup, const QString& permissionId, const QString& value);
    // 设置权限开关
    void getPermissionEnable(const QString& permissionGroup, const QString& permissionId);
    void setPermissionEnable(const QString& permissionGroup, const QString& permissionId, bool enable);

Q_SIGNALS:
    void PermissionEnableChanged(const QString& permissionGroup, const QString& permissionId, bool enable);
    void PermissionInfoChanged();

    void permissionEnableReset(const QString &permissionGroup, const QString &permissionId);
    void permissionInfoReset(const QString &permissionGroup, const QString &permissionId);
    // 数据加载完成
    void permissionInfoLoadFinished(const QString& perInfo);
    // 服务状态回调
    void permissionEnableLoadFinished(const bool loadState);

private:
    DDBusInterface *m_privacyInter;
};

#endif // SECURITYDBUSPROXY_H
