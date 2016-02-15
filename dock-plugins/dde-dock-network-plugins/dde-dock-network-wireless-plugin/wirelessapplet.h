/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef WIRELESSAPPLET_H
#define WIRELESSAPPLET_H

#include <QWidget>
#include <QLabel>
#include "wirelessappletitem.h"
#include "dbus/dbusnetwork.h"

#include <dboxwidget.h>
#include <dswitchbutton.h>
#include <dheaderline.h>
#include <dseparatorhorizontal.h>
#include <dlistwidget.h>

DWIDGET_USE_NAMESPACE

class WirelessApplet : public DVBoxWidget
{
    Q_OBJECT
public:
    explicit WirelessApplet(const QString &uuid, DBusNetwork *dbusNetwork, QWidget *parent = 0);
    ~WirelessApplet();

    int maxStrength();

signals:
    void sizeChanged();
    void activeApChanged(QString ap);

private:
    void initStyleSheet();
    void initTitleLine();
    void initApListContent();
    void removeApFromList(const QString &apPath);
    void addApToList(const WirelessAppletItem::ApData &apData);
    bool removeOverlapApFromList(const WirelessAppletItem::ApData &apData);
    void onAccessPointAdded(const QString &devicePath, const QString &apInfo);
    void onAccessPointRemoved(const QString &devicePath, const QString &apInfo);
    void onApStrengthChanged(int strength);
    void onDevicesChanged();
    void onDeviceEnabledChanged(const QString &path, bool enable);

private:
    QString m_uuid;
    DBusNetwork *m_dbusNetwork;
    QLabel *m_titleLabel;
    DListWidget *m_listWidget;
    DSwitchButton *m_deviceSwitcher;
};

#endif // WIRELESSAPPLET_H
