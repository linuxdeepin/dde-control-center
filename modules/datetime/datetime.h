#ifndef DATETIME_H
#define DATETIME_H

#include "datesettings.h"
#include "contentwidget.h"
#include "datetimeutil.h"

#include "modulewidget.h"
#include "settingsgroup.h"
#include "settingsitem.h"
#include "settingshead.h"
#include "nextpagewidget.h"
#include "switchwidget.h"

#include <QSettings>

#include <types/zoneinfo.h>

using namespace dcc;

namespace dcc {
namespace datetime {

class DatetimeModel;

class Datetime : public ModuleWidget
{
    Q_OBJECT

public:
    explicit Datetime();
    ~Datetime();

    void setModel(const DatetimeModel *model);

signals:
    void editChanged(bool edit);
    void editDatetime();
    void addClick();

    void requestSetNtp(const bool &ntp);
    void requestTimeSettings();
    void requestRemoveUserTimeZone(const ZoneInfo &zone);

public slots:
    void addTimezone(const ZoneInfo &zone);
    void addTimezones(const QList<ZoneInfo> &zones);
    void removeTimezone(const ZoneInfo &zone);

private:
    const DatetimeModel *m_model;

    SettingsGroup *m_timeSettingsGroup;
    SwitchWidget *m_ntpSwitch;
    NextPageWidget *m_timePageButton;

    SettingsGroup *m_timezoneGroup;
    SettingsHead *m_headItem;
    QPushButton *m_addTimezoneButton;
};
}
}
#endif // DATETIME_H
