/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef VPNCONNECTSWIDGET_H
#define VPNCONNECTSWIDGET_H

#include <QPointer>

#include "abstractdevicewidget.h"

class InputPasswordDialog;
class VPNConnectsWidget : public AbstractDeviceWidget
{
    Q_OBJECT
public:
    explicit VPNConnectsWidget(DBusNetwork *dbus, QWidget *parent = 0);

    QString path() const;

private slots:
    void onConnectsChanged();
    void onItemClicked();
    void onClearButtonClicked();
    void onItemStateChanged(int state);
    void onActiveConnectionsChanged();
    void onNeedSecrets(const QString &path, const QString &section,
                       const QString &ssid, bool autoConnect);
    void closeInputDialog();

private:
    QMap<QString, NetworkGenericListItem*> m_mapVpnPathToItem;
    QPointer<InputPasswordDialog> m_ddialog;
    QString m_targetConnectPath;
    QString m_targetConnectSection;
};

#endif // VPNCONNECTSWIDGET_H
