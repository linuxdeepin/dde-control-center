#ifndef DATESETTINGS_H
#define DATESETTINGS_H

#include <QWidget>
#include "clock.h"
#include "datetimewidget.h"

class DateSettings : public QWidget
{
    Q_OBJECT

public:
    explicit DateSettings(QWidget *parent = 0);

public slots:
    void slotMonthChange(DateWidget::Type type, int data);

private:
    Clock *m_clock;
    TimeWidget* m_timeWidget;
    DateWidget* m_yearWidget;
    DateWidget* m_monthWidget;
    DateWidget* m_dayWidget;
};

#endif // DATESETTINGS_H
