/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef TIMEZONEITEMWIDGET_H
#define TIMEZONEITEMWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QMouseEvent>

#include <dthememanager.h>

#include "searchlist.h"

#include "multiaddcheckbutton.h"
#include "dbus/dbustimedate.h"

DWIDGET_USE_NAMESPACE

class TimezoneItemWidget : public QFrame, public SearchItem
{
    Q_OBJECT
public:
    explicit TimezoneItemWidget(const ZoneInfo *m_zoneInfo, QWidget *parent = 0);

    QWidget *widget() const Q_DECL_OVERRIDE {return const_cast<TimezoneItemWidget *>(this);}

    inline const QString zoneName() const {return m_zoneInfo->getZoneName();}
    inline bool selected() const {return m_checkBtn->isChecked();}

signals:
    void clicked();

public slots:
    inline void reloadThemes() {D_THEME_INIT_WIDGET(TimezoneItemWidget);}
    inline void setZones(const QString &zones) {m_zones->setText(zones);}
    inline void setUTCOffset(const QString &offset) {m_utcOffset->setText(offset);}

private:
    QStringList keyWords() const Q_DECL_OVERRIDE {return QStringList();}
    void setData(const QVariant &) Q_DECL_OVERRIDE {}
    QVariant getData() Q_DECL_OVERRIDE {return QVariant();}
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE {m_checkBtn->setChecked(!m_checkBtn->isChecked()); emit clicked(); e->accept();}

private:
    QLabel *m_zones;
    QLabel *m_utcOffset;
    MultiAddCheckButton *m_checkBtn;

    const ZoneInfo *m_zoneInfo;
};

#endif // TIMEZONEITEMWIDGET_H
