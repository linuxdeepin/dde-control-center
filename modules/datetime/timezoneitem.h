#ifndef TIMEZONEITEM_H
#define TIMEZONEITEM_H

#include "settingsitem.h"
#include "datetime/clock.h"
#include "datetime/datetimeutil.h"

#include <QLabel>
#include <QString>
#include <dimagebutton.h>
#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace datetime {

class TimezoneItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit TimezoneItem(QFrame *parent =0);
    void setCity(const Timezone& tz);
    void setDetails(const QString& details);

    void toRemoveMode();
    void toNormalMode();

signals:
    void destroySelf();
    void removeTimezone(const Timezone& tz);

public slots:
    void slotStatus(bool flags);
    void slotRemoveSelf();

private:
    QLabel *m_city;
    QLabel *m_details;
    Clock *m_clock;
    DImageButton* m_removeBtn;
    QHBoxLayout* m_layout;
    QFrame* m_back;
    Timezone m_curTimezone;
};
}
}
#endif // TIMEZONEITEM_H
