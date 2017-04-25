#ifndef DATETIMEMODEL_H
#define DATETIMEMODEL_H

#include <QObject>

#include <types/zoneinfo.h>

namespace dcc {
namespace datetime {

class DatetimeModel : public QObject
{
    Q_OBJECT

public:
    explicit DatetimeModel(QObject* parent =0);

    inline bool nTP() const { return m_ntp; }
    void setNTP(bool ntp);

    QList<ZoneInfo> userTimeZones() const;
    void addUserTimeZone(const ZoneInfo &zone);
    void removeUserTimeZone(const ZoneInfo &zone);
#ifndef DCC_DISABLE_TIMEZONE
    QString systemTimeZoneId() const;
    void setSystemTimeZoneId(const QString &systemTimeZoneId);
#endif
signals:
    void NTPChanged(bool value);
    void userTimeZoneAdded(const ZoneInfo &zone);
    void userTimeZoneRemoved(const ZoneInfo &zone);
    void systemTimeZoneIdChanged(const QString &zone);
    void systemTimeChanged();

private:
    bool m_ntp;
    QStringList m_userZoneIds;
#ifndef DCC_DISABLE_TIMEZONE
    QString m_systemTimeZoneId;
#endif
    QList<ZoneInfo> m_userTimeZones;
};

}
}
#endif // DATETIMEMODEL_H
