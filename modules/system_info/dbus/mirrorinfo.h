#ifndef MIRRORINFO_H
#define MIRRORINFO_H

#include <QObject>
#include <QDBusArgument>
#include <QDebug>
#include <QtDBus>

class MirrorInfo
{
public:
    MirrorInfo();
    static void registerMetaType();

    friend QDebug operator<<(QDebug argument, const MirrorInfo & info);
    friend QDBusArgument &operator<<(QDBusArgument & argument, const MirrorInfo & info);
    friend const QDBusArgument &operator>>(const QDBusArgument & argument, MirrorInfo & info);

public:
    QString m_id;
    QString m_name;
    QString m_url;
};

typedef QList<MirrorInfo> MirrorInfoList;
Q_DECLARE_METATYPE(MirrorInfo)
Q_DECLARE_METATYPE(MirrorInfoList)

#endif // MIRRORINFO_H
