#ifndef APPUPDATEINFO_H
#define APPUPDATEINFO_H

#include <QObject>
#include <QList>
#include <QDBusArgument>
#include <QtDBus>

class AppUpdateInfo
{
public:
    AppUpdateInfo();
    static void registerMetaType();

    friend QDebug operator<<(QDebug argument, const AppUpdateInfo & info);
    friend QDBusArgument &operator<<(QDBusArgument & argument, const AppUpdateInfo & info);
    friend const QDBusArgument &operator>>(const QDBusArgument & argument, AppUpdateInfo & info);

public:
    QString m_packageId;
    QString m_name;
    QString m_icon;
    QString m_currentVersion;
    QString m_avilableVersion;
};

typedef QList<AppUpdateInfo> AppUpdateInfoList;
Q_DECLARE_METATYPE(AppUpdateInfo)
Q_DECLARE_METATYPE(AppUpdateInfoList)

#endif // APPUPDATEINFO_H
