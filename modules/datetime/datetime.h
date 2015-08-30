#ifndef DATETIME_H
#define DATETIME_H

#include <QObject>
#include <QtPlugin>

#include "interfaces.h"
#include "datecontrolwidget.h"
#include "timezonectrlwidget.h"
#include "timezonewidget.h"
#include "timezoneitemwidget.h"
#include "dbus/dbustimedate.h"

#include "searchlist.h"

#include <libdui/dcalendar.h>
#include <libdui/dswitchbutton.h>
#include <libdui/dheaderline.h>

DUI_USE_NAMESPACE

class QFrame;
class Datetime: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "datetime.json")
    Q_INTERFACES(ModuleInterface)

public:
    Datetime();
    ~Datetime() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;

public slots:
    void switchAutoSync(const bool autoSync);

private:
    static const QString getUTCOffset(int offset);
    const QString getZoneCityListByOffset(int zoneOffset);
    const ZoneInfo &getZoneInfoByName(const QString & zoneName) const;
    void showSelectedTimezoneList();
    void showTimezoneList();
    void toRemoveTimezoneMode();

private slots:
    void toggleTimeZone(TimezoneWidget *zone);
    void removeTimeZone(TimezoneWidget *zone);
    void addUserTimeZone();
    void timezoneItemChoosed(const TimezoneItemWidget *item);

private:
    QFrame * m_frame;

    DBusTimedate m_dbusInter;

    QList<ZoneInfo> *m_zoneInfoList;

    DHeaderLine *m_timezoneHeaderLine;
    DCalendar *m_calendar;
    DSwitchButton *m_clockFormatSwitcher;
    DSwitchButton *m_autoSyncSwitcher;
    DateControlWidget *m_dateCtrlWidget;
    TimezoneCtrlWidget *m_timezoneCtrlWidget;
    SearchList *m_timezoneListWidget;

    QList<QString> m_choosedZoneList;
};

#endif //DATETIME_H
