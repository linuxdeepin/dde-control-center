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

class DateSettings : public ContentWidget
{
    Q_OBJECT

public:
    explicit DateSettings(QWidget *parent = 0);

    void setModel(DatetimeModel *model);

signals:
    void requestSetTime(const QDateTime &time);
    void requestSetTimeZone(const QString &zone);

private slots:
    void onCancelButtonClicked();
    void onConfirmButtonClicked();

    void setTimeZone(const QString &zone);

private:
    DatetimeModel *m_model;

    SettingsGroup *m_datetimeGroup;

    ClockItem *m_clock;
    TimeWidget *m_timeWidget;
    DateWidget *m_yearWidget;
    DateWidget *m_monthWidget;
    DateWidget *m_dayWidget;
    QPushButton *m_cancelButton;
    QPushButton *m_confirmButton;

    SettingsGroup *m_timezoneGroup;
    NextPageWidget *m_timezoneItem;
};
}
}
#endif // DATESETTINGS_H
