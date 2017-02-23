#ifndef DATESETTINGS_H
#define DATESETTINGS_H

#include <QWidget>

#include <types/zoneinfo.h>

#include "contentwidget.h"

class QPushButton;

namespace dcc {
namespace widgets {
class SettingsGroup;
class NextPageWidget;
class ButtonTuple;
}
}

using namespace dcc;
using namespace dcc::widgets;

namespace dcc {
namespace datetime {

class DatetimeModel;
class TimeWidget;
class DateWidget;
class ClockItem;
class TimeZoneChooser;

class DateSettings : public ContentWidget
{
    Q_OBJECT

public:
    explicit DateSettings(QWidget *parent = 0);

signals:
    void requestSetTime(const QDateTime &time);

private slots:
    void onCancelButtonClicked();
    void onConfirmButtonClicked();

    void updateDayRange();

private:
    SettingsGroup *m_datetimeGroup;

    ClockItem *m_clock;
    TimeWidget *m_timeWidget;
    DateWidget *m_yearWidget;
    DateWidget *m_monthWidget;
    DateWidget *m_dayWidget;

    dcc::widgets::ButtonTuple *m_buttonTuple;
};

}
}
#endif // DATESETTINGS_H
