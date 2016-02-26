/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef MODEMNETWORKLISTITEM_H
#define MODEMNETWORKLISTITEM_H

#include "abstractdevicewidget.h"

class ModemNetworkListItem : public AbstractDeviceWidget
{
    Q_OBJECT
public:
    explicit ModemNetworkListItem(DBusNetwork *dbus, QWidget *parent = 0);

private slots:
    void onConnectsChanged();
    void init();

private:
    NetworkGenericListItem *m_item = nullptr;
};

#endif // MODEMNETWORKLISTITEM_H
