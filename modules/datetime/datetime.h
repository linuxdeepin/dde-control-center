#ifndef DATETIME_H
#define DATETIME_H

#include <QObject>
#include <QtPlugin>

#include "interfaces.h"
#include "dbus/dbustimedate.h"

#include <libdui/dcalendar.h>
#include <libdui/dswitchbutton.h>

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
    void switchClockFormat(const bool use24HourFormat);

private:
    QFrame * m_frame;

    DBusTimedate m_dbusInter;

    DUI::DCalendar *m_calendar;
    DUI::DSwitchButton *m_clockFormatSwitcher;
    DUI::DSwitchButton *m_autoSyncSwitcher;
};

#endif //DATETIME_H
