/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WIRELESSPAGE_H
#define WIRELESSPAGE_H

#include "contentwidget.h"

#include <QMap>
#include <QTimer>
#include <QPointer>
#include <QJsonObject>

namespace dcc {

namespace widgets {
class SettingsGroup;
class SwitchWidget;
}

namespace network {

class ConnectHiddenPage;
class ConnectionEditPage;
class AccessPointWidget;
class NetworkModel;
class WirelessDevice;
class WirelessPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit WirelessPage(WirelessDevice *dev, QWidget *parent = 0);
    ~WirelessPage();

    void setModel(NetworkModel *model);

signals:
    void requestEditAP(const QString &devPath, const QString &uuid) const;
    void requestConnectAp(const QString &devPath, const QString &apPath, const QString &uuid) const;
    void requestCreateApConfig(const QString &devPath, const QString &apPath) const;
    void requestCreateAp(const QString &type, const QString &devPath) const;
    void requestDeleteConnection(const QString &uuid);
    void requestDisconnectConnection(const QString &uuid);
    void requestNextPage(ContentWidget * const w) const;
    void requestDeviceRemanage(const QString &devPath) const;
    void requestDeviceAPList(const QString &devPath) const;
    void requestDeviceEnabled(const QString &devPath, const bool enabled) const;
    void requestFrameKeepAutoHide(const bool autoHide) const;

public slots:
    void onAPAdded(const QJsonObject &apInfo);
    void onAPChanged(const QJsonObject &apInfo);
    void onAPRemoved(const QString &ssid);
    void onHotspotEnableChanged(const bool enabled);
    void onCloseHotspotClicked();

private slots:
    void onDeviceRemoved();
    void sortAPList();
    void onApWidgetEditRequested(const QString &path, const QString &ssid);
    void onApWidgetConnectRequested(const QString &path, const QString &ssid);
    void showConnectHidePage();
    void showAPEditPage(const QString &session);

private:
    void updateActiveAp();

private:
    WirelessDevice *m_device;
    NetworkModel *m_model;

    dcc::widgets::SettingsGroup *m_listGroup;
    dcc::widgets::SettingsGroup *m_tipsGroup;
    AccessPointWidget *m_connectHideSSID;
    QPushButton *m_closeHotspotBtn;

    QPointer<ConnectionEditPage> m_apEditPage;

    QString m_editingUuid;
    QTimer m_sortDelayTimer;
    QMap<QString, AccessPointWidget *> m_apItems;
};

}   // namespace dcc

}   // namespace network

#endif // WIRELESSPAGE_H
