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

#ifndef PRIVACYSECURITYMODEL_H
#define PRIVACYSECURITYMODEL_H

#include <QObject>
#include <QMap>

class ServiceControlItems;

namespace DCC_PRIVACY_NAMESPACE {
enum ServiceCategory : int{
    Camera,
    Microphone,
    UserFolders,
    Calendar,
    Screenshots
};

struct DATE {
    QString name;
    QString displayName;
    QString icon;
    ServiceCategory category;
    DATE(const QString &_name, const QString &_displayName, const QString _icon, ServiceCategory _category)
        : name(_name)
        , displayName(_displayName)
        , icon(_icon)
        , category(_category)
    {}
};
}

class PrivacySecurityModel : public QObject
{
    Q_OBJECT
public:
    explicit PrivacySecurityModel(QObject *parent = nullptr);

    void initServiceItems();

    QString getDaemonDefineName(DCC_PRIVACY_NAMESPACE::ServiceCategory category);

    QList<DCC_PRIVACY_NAMESPACE::DATE> getModuleInfo();
    QMap<QString, DCC_PRIVACY_NAMESPACE::ServiceCategory> getServiceVategory() { return m_serviceToCategory; }

    DCC_PRIVACY_NAMESPACE::ServiceCategory getCategory(const QString &serviceName);

    // TODO: 后期需要加组信息，确认返回的Item
    ServiceControlItems* getServiceItem(const QString &daemonDefineName);
    ServiceControlItems* getServiceItem(DCC_PRIVACY_NAMESPACE::ServiceCategory category);

    bool findService();
    void clearServiceItemDate();

private:
    QMap<QString, DCC_PRIVACY_NAMESPACE::ServiceCategory> m_serviceToCategory;
    QList<DCC_PRIVACY_NAMESPACE::DATE> m_moduleInfo;

    QList<ServiceControlItems*> m_groupService;
//    QMap<QString, QString> m_appListInconPath; // 应用图标路径
};

#endif // PRIVACYSECURITYMODEL_H
