//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "servicecontrolitems.h"

#include <QDebug>

using namespace DCC_PRIVACY_NAMESPACE;
ServiceControlItems::ServiceControlItems(ServiceCategory category, QObject *parent)
    : QObject(parent)
    , m_currentCategory(category)
    , m_available(false)
    , m_switch(true)
{

}

ServiceControlItems::~ServiceControlItems()
{

}

void ServiceControlItems::setServiceGroup(const QString &group)
{
    m_currentGroup = group;
}

void ServiceControlItems::setServiceAvailable(const bool available)
{
    if (available != m_available) {
        m_available = available;
        Q_EMIT serviceAvailableStateChange(available);
    }
}

void ServiceControlItems::setSwitchState(const bool switchState)
{
    if (m_switch != switchState) {
        m_switch = switchState;
        Q_EMIT serviceSwitchStateChange(switchState);
    }
}

void ServiceControlItems::setServiceApps(const QList<App> &apps)
{
    qDebug() << " 数据更新： setServiceApps: " << m_currentCategory << apps.size() ;
    if (m_appList != apps) {
        m_appList.clear();
        m_appList.append(apps);
        Q_EMIT serviceAppsDateChange();
    }
}

QList<App> ServiceControlItems::getServiceApps()
{
    return m_appList;
}

void ServiceControlItems::clearServiceApps()
{
    m_appList.clear();
    setServiceAvailable(false);
    Q_EMIT serviceAppsDateChange();
}

void ServiceControlItems::setPermissionInfo(const QString &name, const QString &visible)
{
    Q_EMIT permissionInfoChange(name, visible);
}
