/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef WIRELESSNETWORKLISTITEM_H
#define WIRELESSNETWORKLISTITEM_H

#include <QPointer>
#include <QJsonArray>

#include "abstractdevicewidget.h"

class InputPasswordDialog;
class WirelessNetworkListItem : public AbstractDeviceWidget
{
    Q_OBJECT
public:
    explicit WirelessNetworkListItem(DBusNetwork *dbus, QWidget *parent = 0);
    ~WirelessNetworkListItem();

private slots:
    void onItemClicked();
    void onConnectsChanged();
    void updateItemIndex();
    void updateActiveAp();
    void onActiveConnectionsChanged();
    NetworkGenericListItem *addAccessPoint(const QVariantMap &map, bool auto_sort = false);

private:
    void init();

    void updateItemIndex(NetworkGenericListItem *item);
    void closeInputDialog();

    void updateAllItemUuid();
    void updateItemUuid(NetworkGenericListItem *item);

    QPointer<NetworkGenericListItem> m_activeItem;
    QPointer<InputPasswordDialog> m_ddialog;
    QMap<QString, NetworkGenericListItem*> m_mapApPathToItem;
    QMap<QString, NetworkGenericListItem*> m_mapApSsidToItem;
    QMap<QString, NetworkGenericListItem*> m_mapApUuidToItem;
    QJsonArray connectsArray;

    QString m_targetConnectPath;
    QString m_targetConnectSection;
};

#endif // WIRELESSNETWORKLISTITEM_H
