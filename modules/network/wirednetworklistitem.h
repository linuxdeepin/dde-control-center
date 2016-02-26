/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef WIREDNETWORKLISTITEM_H
#define WIREDNETWORKLISTITEM_H

#include "abstractdevicewidget.h"

class WiredNetworkListItem : public AbstractDeviceWidget
{
    Q_OBJECT
public:
    explicit WiredNetworkListItem(DBusNetwork *dbus, QWidget *parent = 0);

private slots:
    void init();
    void onConnectsChanged();
    void onItemClicked();
    void onClearButtonClicked();
    void onItemStateChanged(int state);

private:
    QMap<QString, NetworkGenericListItem *> m_mapPppoePathToItem;
};

#endif // WIREDNETWORKLISTITEM_H
