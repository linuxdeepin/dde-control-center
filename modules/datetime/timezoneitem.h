#ifndef TIMEZONEITEM_H
#define TIMEZONEITEM_H

#include "settingsitem.h"
#include <QLabel>
#include <QString>

#include "clock.h"

using namespace dcc;

class TimezoneItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit TimezoneItem(QFrame *parent =0);
    void setTimeZone(const QString& timezone);
    void setCity(const QString& city);
    void setDetails(const QString& details);

private:
    QLabel *m_city;
    QLabel *m_details;
    QString m_timezone;
    Clock *m_clock;
};

#endif // TIMEZONEITEM_H
