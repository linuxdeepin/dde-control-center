#include "appupdateinfo.h"

AppUpdateInfo::AppUpdateInfo()
{

}

void AppUpdateInfo::registerMetaType()
{
    qRegisterMetaType<AppUpdateInfo>();
    qDBusRegisterMetaType<AppUpdateInfo>();
    qRegisterMetaType<AppUpdateInfoList>();
    qDBusRegisterMetaType<AppUpdateInfoList>();
}

const QDBusArgument &operator>>(const QDBusArgument &argument, AppUpdateInfo &info)
{
    argument.beginStructure();
    argument >> info.m_packageId;
    argument >> info.m_name;
    argument >> info.m_icon;
    argument >> info.m_currentVersion;
    argument >> info.m_avilableVersion;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const AppUpdateInfo &info)
{
    argument.beginStructure();
    argument << info.m_packageId;
    argument << info.m_name;
    argument << info.m_icon;
    argument << info.m_currentVersion;
    argument << info.m_avilableVersion;
    argument.endStructure();

    return argument;
}

QDebug operator<<(QDebug argument, const AppUpdateInfo &info)
{
    argument << "packageId: " << info.m_packageId;
    argument << "name: " << info.m_name;
    argument << "icon: " << info.m_icon;
    argument << "currentVer: " << info.m_currentVersion;
    argument << "avilableVer: " << info.m_avilableVersion;

    return argument;
}
