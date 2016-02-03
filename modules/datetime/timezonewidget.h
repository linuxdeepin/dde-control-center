/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef TIMEZONEWIDGET_H
#define TIMEZONEWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QMouseEvent>

#include "multideletebutton.h"
#include "multiaddcheckbutton.h"
#include "dbus/dbustimedate.h"

#include "searchlist.h"

#include <libdui/dimagebutton.h>

DUI_USE_NAMESPACE

class TimezoneWidget : public QFrame, public SearchItem
{
    Q_OBJECT
    Q_PROPERTY(bool selected READ isSelected NOTIFY selectStateChanged)

public:
    explicit TimezoneWidget(const ZoneInfo *m_zoneInfo, QWidget *parent = 0);

    inline bool isSelected() {return m_selected;}
    inline const QString zoneName() {return m_zoneInfo->getZoneName();}

    QWidget *widget() const Q_DECL_OVERRIDE {return const_cast<TimezoneWidget *>(this);}

signals:
    void clicked();
    void selectStateChanged(bool isSelected);

public slots:
    void setSelected(const bool selected);
    inline void setZoneCities(const QString & cities) {m_citiesLabel->setText(cities);}
    inline void setZoneUTCOffset(const QString & UTCOffset) {m_utcOffsetLabel->setText(UTCOffset);}
    inline void removeMode() {m_removeBtn->setVisible(!m_selected); m_selectedBtn->setVisible(m_selected);}
    inline void normalMode() {m_removeBtn->setVisible(false); m_selectedBtn->setVisible(m_selected);}

private:
    inline void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE {if (!m_selected) emit clicked(); e->accept();}
    QStringList keyWords() const Q_DECL_OVERRIDE {return QStringList();}
    void setData(const QVariant&) Q_DECL_OVERRIDE {}
    QVariant getData() Q_DECL_OVERRIDE {return QVariant();}

private:
    const ZoneInfo *m_zoneInfo;
    QLabel *m_citiesLabel;
    QLabel *m_utcOffsetLabel;
    MultiDeleteButton *m_removeBtn;
    DImageButton *m_selectedBtn;

    bool m_selected = false;
};

#endif // TIMEZONEWIDGET_H
