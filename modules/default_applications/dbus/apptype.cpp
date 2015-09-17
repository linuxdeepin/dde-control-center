#include "apptype.h"

QDBusArgument &operator<<(QDBusArgument &argument, const AppType &message)
{
    argument.beginStructure();
    argument << message.m_appDesktop;
    argument << message.m_appName;
    argument << message.m_command;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, AppType &message)
{
    argument.beginStructure();
    argument >> message.m_appDesktop;
    argument >> message.m_appName;
    argument >> message.m_command;
    argument.endStructure();

    return argument;
}

void AppType::registerMetaType()
{
    qRegisterMetaType<AppType>();
    qDBusRegisterMetaType<AppType>();
    qRegisterMetaType<AppList>();
    qDBusRegisterMetaType<AppList>();
}

bool AppType::operator==(const AppType &what) const
{
    return what.m_appDesktop == m_appDesktop &&
           what.m_appName == m_appName &&
           what.m_command == m_command;
}

QDebug &operator<<(QDebug &argument, const AppType &message)
{
    argument << message.m_appDesktop;
    argument << message.m_appName;
    argument << message.m_command;

    return argument;
}
