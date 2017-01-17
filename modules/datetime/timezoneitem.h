#ifndef TIMEZONEITEM_H
#define TIMEZONEITEM_H

#include "settingsitem.h"
#include "datetime/clock.h"

#include <QLabel>
#include <QString>
#include <dimagebutton.h>
#include <QHBoxLayout>

#include <types/zoneinfo.h>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace datetime {

class TimezoneItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit TimezoneItem(QFrame *parent =0);

    inline ZoneInfo timeZone() const { return m_timezone; }
    void setTimeZone(const ZoneInfo &info);

    void toRemoveMode();
    void toNormalMode();

    void updateInfo();

signals:
    void removeClicked();

private:
    ZoneInfo m_timezone;

    QLabel *m_city;
    QLabel *m_details;
    Clock *m_clock;
    DImageButton* m_removeBtn;
};
}
}
#endif // TIMEZONEITEM_H
