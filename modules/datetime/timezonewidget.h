#ifndef TIMEZONEWIDGET_H
#define TIMEZONEWIDGET_H

#include <QFrame>
#include <QLabel>

#include "multideletebutton.h"
#include "multiaddcheckbutton.h"
#include "dbus/dbustimedate.h"

#include <libdui/dimagebutton.h>

DUI_USE_NAMESPACE

class TimezoneWidget : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool selected READ isSelected)

public:
    explicit TimezoneWidget(const ZoneInfo *m_zoneInfo, QWidget *parent = 0);

    inline bool isSelected() {return m_selected;}
    inline const QString zoneName() {return m_zoneInfo->m_zoneName;}

signals:
    void clicked();
    void selectStateChanged(bool isSelected);

public slots:
    void setSelected(const bool selected);
    inline void reloadThemes() {D_THEME_INIT_WIDGET(TimezoneWidget);}
    inline void setZoneCities(const QString & cities) {m_citiesLabel->setText(cities);}
    inline void setZoneUTCOffset(const QString & UTCOffset) {m_utcOffsetLabel->setText(UTCOffset);}
    inline void removeMode() {m_removeBtn->setVisible(!m_selected); m_selectedBtn->setVisible(m_selected);}
    inline void normalMode() {m_removeBtn->setVisible(false); m_selectedBtn->setVisible(m_selected);}

private:
    inline void mouseReleaseEvent(QMouseEvent *) {if (!m_selected) emit clicked();}

private:
    const ZoneInfo *m_zoneInfo;
    QLabel *m_citiesLabel;
    QLabel *m_utcOffsetLabel;
    MultiDeleteButton *m_removeBtn;
    DImageButton *m_selectedBtn;

    bool m_selected = false;
};

#endif // TIMEZONEWIDGET_H
