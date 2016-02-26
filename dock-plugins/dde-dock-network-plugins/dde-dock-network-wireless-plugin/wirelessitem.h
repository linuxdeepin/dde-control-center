/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef WIRELESSITEM_H
#define WIRELESSITEM_H

#include <QLabel>
#include "wirelessapplet.h"
#include "dbus/dbusnetwork.h"
#include "../network-data/networkdata.h"

class WirelessItem : public QLabel
{
    Q_OBJECT
public:
    explicit WirelessItem(const QString &uuid, DBusNetwork *dbusNetwork, QWidget *parent = 0);
    WirelessApplet *applet() const;

signals:
    void appletSizeChanged();

private:
    void updateIcon();

private:
    QString m_uuid;
    DBusNetwork *m_dbusNetwork;
    WirelessApplet *m_applet;
};

#endif // WIRELESSITEM_H
