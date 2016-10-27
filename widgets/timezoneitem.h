#ifndef TIMEZONEITEM_H
#define TIMEZONEITEM_H

#include "settingsitem.h"
#include "datetime/clock.h"

#include <QLabel>
#include <QString>
#include <dimagebutton.h>
#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE

namespace dcc {

class TimezoneItem : public SettingsItem
{
    Q_OBJECT

public:
    explicit TimezoneItem(QFrame *parent =0);
    void setTimeZone(const QString& timezone);
    void setCity(const QString& city);
    void setDetails(const QString& details);

    void toRemoveMode();
    void toNormalMode();

signals:
    void destroySelf();

public slots:
    void slotStatus(bool flags);
    void slotRemoveSelf();

private:
    QLabel *m_city;
    QLabel *m_details;
    QString m_timezone;
    Clock *m_clock;
    DImageButton* m_removeBtn;
    QHBoxLayout* m_layout;
    QFrame* m_back;
};

}
#endif // TIMEZONEITEM_H
