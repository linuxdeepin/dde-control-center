#ifndef TIMEZONEITEMWIDGET_H
#define TIMEZONEITEMWIDGET_H

#include <QFrame>
#include <QLabel>

#include <libdui/dthememanager.h>

#include "multiaddcheckbutton.h"
#include "dbus/dbustimedate.h"

DUI_USE_NAMESPACE

class TimezoneItemWidget : public QFrame
{
    Q_OBJECT
public:
    explicit TimezoneItemWidget(const ZoneInfo *m_zoneInfo, QWidget *parent = 0);

    inline const QString zoneName() const {return m_zoneInfo->m_zoneName;}
    inline bool selected() const {return m_checkBtn->isChecked();}

public slots:
    inline void reloadThemes() {D_THEME_INIT_WIDGET(TimezoneItemWidget);}
    inline void setZones(const QString & zones) {m_zones->setText(zones);}
    inline void setUTCOffset(const QString & offset) {m_utcOffset->setText(offset);}

private:
    QLabel *m_zones;
    QLabel *m_utcOffset;
    MultiAddCheckButton *m_checkBtn;

    const ZoneInfo *m_zoneInfo;
};

#endif // TIMEZONEITEMWIDGET_H
