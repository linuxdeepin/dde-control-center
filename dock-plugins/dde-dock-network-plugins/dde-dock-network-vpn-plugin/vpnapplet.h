/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef VPNAPPLET_H
#define VPNAPPLET_H

#include <QWidget>
#include <QLabel>
#include "vpnappletitem.h"
#include "dbus/dbusnetwork.h"

#include <dboxwidget.h>
#include <dswitchbutton.h>
#include <dheaderline.h>
#include <dseparatorhorizontal.h>
#include <dlistwidget.h>

DWIDGET_USE_NAMESPACE

class VpnApplet : public DVBoxWidget
{
    Q_OBJECT
public:
    explicit VpnApplet(DBusNetwork *dbusNetwork, QWidget *parent = 0);
    ~VpnApplet();

signals:
    void appletSizeChanged();

private:
    void initStyleSheet();
    void initTitleLine();
    void initVpnListContent();
    void addItemToList(const VpnAppletItem::VpnData &vpnData);
    void removeItemFromList(const QString &uuid);
    void onConnectionsChanged();
    void onVpnEnableChanged();

private:
    QString m_uuid;
    QStringList m_uuidList;
    DBusNetwork *m_dbusNetwork;
    QLabel *m_titleLabel;
    DListWidget *m_listWidget;
    DSwitchButton *m_vpnSwitcher;
};

#endif // VPNAPPLET_H
