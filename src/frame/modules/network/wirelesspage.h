/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             listenerri <listenerri@gmail.com>
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

#include "widgets/contentwidget.h"

#include <QMap>
#include <QTimer>
#include <QPointer>
#include <QJsonObject>

namespace dde {
namespace network {
class NetworkModel;
class NetworkDevice;
class WirelessDevice;
}
}

namespace dcc {

namespace widgets {
class SettingsGroup;
class SwitchWidget;
}

namespace network {

class ConnectionWirelessEditPage;
class AccessPointWidget;
class WirelessPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit WirelessPage(dde::network::WirelessDevice *dev, QWidget *parent = nullptr);
    ~WirelessPage();

    void setModel(dde::network::NetworkModel *model);

Q_SIGNALS:
    void requestConnectAp(const QString &devPath, const QString &apPath, const QString &uuid) const;
    void requestDisconnectConnection(const QString &uuid);
    void requestNextPage(ContentWidget * const w) const;
    void requestDeviceRemanage(const QString &devPath) const;
    void requestDeviceAPList(const QString &devPath) const;
    void requestWirelessScan();
    void requestDeviceEnabled(const QString &devPath, const bool enabled) const;
    void requestFrameKeepAutoHide(const bool autoHide) const;
    void requestShowAPEditPage(dde::network::NetworkDevice *device, const QString &session) const;
    void requestRemoveAPEditPage(dde::network::NetworkDevice *device) const;

public Q_SLOTS:
    void onAPAdded(const QJsonObject &apInfo);
    void onAPChanged(const QJsonObject &apInfo);
    void onAPRemoved(const QJsonObject &apInfo);
    void onHotspotEnableChanged(const bool enabled);
    void onCloseHotspotClicked();

private Q_SLOTS:
    void sortAPList();
    void onApWidgetEditRequested(const QString &apPath, const QString &ssid);
    void onApWidgetConnectRequested(const QString &path, const QString &ssid);
    void showConnectHidePage();
    void onDeviceRemoved();
    void onActivateApFailed(const QString &apPath, const QString &uuid);
    void refreshLoadingIndicator();

private:
    void updateActiveAp();
    QString connectionUuid(const QString &ssid);

private:
    dde::network::WirelessDevice *m_device;
    dde::network::NetworkModel *m_model;

    dcc::widgets::SettingsGroup *m_listGroup;
    dcc::widgets::SettingsGroup *m_tipsGroup;
    AccessPointWidget *m_connectHideSSID;
    QPushButton *m_closeHotspotBtn;
    AccessPointWidget *m_currentClickApw;

    QPointer<ConnectionWirelessEditPage> m_apEditPage;

    QString m_editingUuid;
    QTimer *m_sortDelayTimer;
    QTimer *m_indicatorDelayTimer;
    QMap<QString, AccessPointWidget *> m_apItems;
};

}   // namespace dcc

}   // namespace network

#endif // WIRELESSPAGE_H
