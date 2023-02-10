//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
