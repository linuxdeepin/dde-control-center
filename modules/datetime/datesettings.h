#ifndef DATESETTINGS_H
#define DATESETTINGS_H

#include <QWidget>
#include "clock.h"
#include "datetimewidget.h"
#include "settingshead.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "contentwidget.h"

class DateSettings : public ContentWidget
{
    Q_OBJECT

public:
    explicit DateSettings(QWidget *parent = 0);

    void setNTP(bool ntp);

signals:
    void dateChanged(int year, int month, int day, int hour, int minute);
    void autoSyncChanged(const bool checked);

public slots:
    void slotMonthChange(DateWidget::Type type, int data);
    void slotConfirm();
    void slotCancel();
    void slotAutoSync(const bool checked);

private:
    Clock *m_clock;
    SwitchWidget* m_autoItem;
    TimeButton* m_timeBtn;
    TimeWidget* m_timeWidget;
    DateWidget* m_dayWidget;
    DateWidget* m_yearWidget;
    DateWidget* m_monthWidget;

    SettingsGroup *m_okGroup;
};

#endif // DATESETTINGS_H
