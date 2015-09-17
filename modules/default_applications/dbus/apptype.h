#ifndef APPTYPE_H
#define APPTYPE_H

#include <QtDBus/QtDBus>
#include <QtDBus/QDBusArgument>

class AppType
{
public:
    static void registerMetaType();

    friend QDBusArgument &operator<<(QDBusArgument &argument, const AppType& message);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, AppType &message);
    friend QDebug &operator<<(QDebug &argument, const AppType& message);

    bool operator==(const AppType &what) const;

public:
    QString m_appDesktop;
    QString m_appName;
    QString m_command;
};

typedef QList<AppType> AppList;

Q_DECLARE_METATYPE(AppType)
Q_DECLARE_METATYPE(AppList)

#endif // APPTYPE_H
