//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef ZONEINFO_H
#define ZONEINFO_H

#include <QDebug>
#include <QDataStream>
#include <QString>
#include <QDBusArgument>
#include <QDBusMetaType>

class ZoneInfo
{
    Q_GADGET
    Q_PROPERTY(QString zoneName READ getZoneName)
    Q_PROPERTY(QString city READ getZoneCity)
    Q_PROPERTY(int utcOffset READ getUTCOffset)
public:
    ZoneInfo();

    friend QDebug operator<<(QDebug argument, const ZoneInfo &info);
    friend QDBusArgument &operator<<(QDBusArgument &argument, const ZoneInfo &info);
    friend QDataStream &operator<<(QDataStream &argument, const ZoneInfo &info);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, ZoneInfo &info);
    friend const QDataStream &operator>>(QDataStream &argument, ZoneInfo &info);

    bool operator==(const ZoneInfo &what) const;

public:
    inline QString getZoneName() const { return m_zoneName; }
    inline QString getZoneCity() const { return m_zoneCity; }
    inline int getUTCOffset() const { return m_utcOffset; }
    QString getUtcOffsetText() const
    {
        QString gmData;
        int utcOff = m_utcOffset / 3600;
        if (utcOff >= 0) {
            gmData = QString("(UTC+%1:%2)").arg(utcOff, 2, 10, QLatin1Char('0')).arg(m_utcOffset % 3600 / 60, 2, 10, QLatin1Char('0'));
        } else {
            gmData = QString("(UTC%1:%2)").arg(utcOff, 3, 10, QLatin1Char('0')).arg(m_utcOffset % 3600 / 60, 2, 10, QLatin1Char('0'));
        }

        return gmData;
    }

private:
    QString m_zoneName;
    QString m_zoneCity;
    int m_utcOffset;
    qint64 i2;
    qint64 i3;
    int i4;
};

Q_DECLARE_METATYPE(ZoneInfo)

void registerZoneInfoMetaType();

#endif // ZONEINFO_H
