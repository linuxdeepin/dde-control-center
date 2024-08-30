//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SERVICECONTROLITEMS_H
#define SERVICECONTROLITEMS_H

#include <QObject>
#include "privacysecuritymodel.h"

// 每个App信息： 目前只存在 名称 状态
struct App {
    QString m_name;
    QString m_enable;

    bool operator ==(const App &app) const {
        return app.m_name == m_name && app.m_enable == m_enable;
    }

    bool operator !=(const App &app) const {
        return app.m_name != m_name || app.m_enable != m_enable;
    }
};

/*
服务管控项： 用于创建每个服务项
*/

class ServiceControlItems : public QObject
{
    Q_OBJECT
public:
    explicit ServiceControlItems(DCC_PRIVACY_NAMESPACE::ServiceCategory category, QObject *parent = nullptr);
    ~ServiceControlItems();

public:
    // 设置组 仅一次
    void setServiceGroup(const QString &group);
    QString getServiceGroup() { return m_currentGroup; }

    void setServiceAvailable(const bool available);
    bool getServiceAvailable() { return m_available; }

    void setSwitchState(const bool switchState);
    bool getSwitchState() { return m_switch; }

    void setServiceApps(const QList<App> &apps);
    QList<App> getServiceApps();

    void clearServiceApps();

    DCC_PRIVACY_NAMESPACE::ServiceCategory currentCategory() { return m_currentCategory; }

    void setPermissionInfo(const QString& name, const QString& visible);

//    PrivacySecurityModel::ServiceCategory currentCategory() { return m_currentCategory; }
signals:
    void serviceAppsDateChange();
    // 服务是否存在
    void serviceAvailableStateChange(const bool available);
    // 权限总开关
    void serviceSwitchStateChange(const bool switchState);
    // 应用数据变化
    void permissionInfoChange(const QString& name, const QString& visible);

private:
    QString m_currentGroup;
    DCC_PRIVACY_NAMESPACE::ServiceCategory m_currentCategory;
    bool m_available;
    bool m_switch;
    QList<App> m_appList;
};

#endif // SERVICECONTROLITEMS_H
