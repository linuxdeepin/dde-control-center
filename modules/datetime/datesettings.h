#ifndef DATESETTINGS_H
#define DATESETTINGS_H

#include <QWidget>
#include "clock.h"
#include "datetimewidget.h"
#include "settingshead.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "contentwidget.h"
#include "nextpagewidget.h"
#include "datetimeutil.h"

using namespace dcc;

class DateSettings : public ContentWidget
{
    Q_OBJECT

public:
    explicit DateSettings(QWidget *parent = 0);

    void setNTP(bool ntp);

signals:
    void dateChanged(int year, int month, int day, int hour, int minute);
    void autoSyncChanged(const bool checked);
    void changeClick();

public slots:
    void slotMonthChange(DateWidget::Type type, int data);
    void slotConfirm();
    void slotCancel();
    void slotAutoSync(const bool checked);
    void setTimezone(const QString &timezone);

private:
    Clock *m_clock;
    SwitchWidget* m_autoItem;
    TimeButton* m_timeBtn;
    TimeWidget* m_timeWidget;
    DateWidget* m_dayWidget;
    DateWidget* m_yearWidget;
    DateWidget* m_monthWidget;
    NextPageWidget* m_timezoneItem;
    SettingsGroup *m_okGroup;
};

#endif // DATESETTINGS_H
