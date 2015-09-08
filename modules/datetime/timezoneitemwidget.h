#ifndef TIMEZONEITEMWIDGET_H
#define TIMEZONEITEMWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QMouseEvent>

#include <libdui/dthememanager.h>

#include "searchlist.h"

#include "multiaddcheckbutton.h"
#include "dbus/dbustimedate.h"

DUI_USE_NAMESPACE

class TimezoneItemWidget : public QFrame, public SearchItem
{
    Q_OBJECT
public:
    explicit TimezoneItemWidget(const ZoneInfo *m_zoneInfo, QWidget *parent = 0);

    QWidget *widget() const Q_DECL_OVERRIDE {return const_cast<TimezoneItemWidget *>(this);}

    inline const QString zoneName() const {return m_zoneInfo->m_zoneName;}
    inline bool selected() const {return m_checkBtn->isChecked();}

signals:
    void clicked();

public slots:
    inline void reloadThemes() {D_THEME_INIT_WIDGET(TimezoneItemWidget);}
    inline void setZones(const QString & zones) {m_zones->setText(zones);}
    inline void setUTCOffset(const QString & offset) {m_utcOffset->setText(offset);}

private:
    QStringList keyWords() const Q_DECL_OVERRIDE {return QStringList();}
    void setData(const QVariant&) Q_DECL_OVERRIDE {}
    QVariant getData() Q_DECL_OVERRIDE {return QVariant();}
    void mouseReleaseEvent(QMouseEvent *e) {emit clicked(); m_checkBtn->setChecked(!m_checkBtn->isChecked()); e->accept();}

private:
    QLabel *m_zones;
    QLabel *m_utcOffset;
    MultiAddCheckButton *m_checkBtn;

    const ZoneInfo *m_zoneInfo;
};

#endif // TIMEZONEITEMWIDGET_H
