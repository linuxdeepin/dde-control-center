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
#include "interface/namespace.h"

#include <wirelessdevice.h>
#include <DStyleOption>
#include <DStyleHelper>
#include <DListView>
#include <DSpinner>

#include <QPointer>

QT_BEGIN_NAMESPACE
class QTimer;
class QListView;
class QVBoxLayout;
class QStandardItemModel;
QT_END_NAMESPACE

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
}

namespace DCC_NAMESPACE {
namespace network {
class ConnectionWirelessEditPage;
struct APSortInfo {
    int signalstrength;
    QString ssid;
    bool connected;
    bool operator <(const APSortInfo &other)
    {
        if (connected ^ other.connected) {
            return !connected;
        }
        if (signalstrength != other.signalstrength) {
            return signalstrength < other.signalstrength;
        }
        return ssid < other.ssid;
    }
};

class APItem : public DStandardItem
{
public:
    explicit APItem(const QString &text, QStyle *style, DTK_WIDGET_NAMESPACE::DListView *parent = nullptr);
    virtual ~APItem();
    void setSecure(bool isSecure);
    bool secure() const;
    void setSignalStrength(int ss);
    int signalStrength() const;
    void setConnected(bool connected);
    bool isConnected();
    void setSortInfo(const APSortInfo &si);
    APSortInfo sortInfo();
    void setPath(const QString &path);
    QString path() const;
    void setUuid(const QString &uuid);
    QString uuid() const;
    QAction *action() const;
    bool operator<(const QStandardItem &other) const override;
    bool setLoading(bool isLoading);
public:
    enum {
        SortRole = Dtk::UserRole + 1,
        PathRole,
        SecureRole
    };
private:
    DListView *m_parentView;
    DTK_WIDGET_NAMESPACE::DStyleHelper m_dStyleHelper;
    DViewItemAction *m_secureAction;
    bool m_preLoading;
    QString m_uuid;
    QPointer<DViewItemAction> m_loadingAction;
    QPointer<DViewItemAction> m_arrowAction;
    QPointer<DTK_WIDGET_NAMESPACE::DSpinner> m_loadingIndicator;
};

class WirelessPage : public dcc::ContentWidget
{
    Q_OBJECT
public:
    enum WifiStatus {
        Wifi_Unknown = 0,
        Wifi_Available,
        Wifi_Unavailable
    };

    explicit WirelessPage(dde::network::WirelessDevice *dev, QWidget *parent = nullptr);
    virtual ~WirelessPage();

    void setModel(dde::network::NetworkModel *model);
    void jumpByUuid(const QString &uuid);
Q_SIGNALS:
    void requestConnectAp(const QString &devPath, const QString &apPath, const QString &uuid) const;
    void requestDisconnectConnection(const QString &uuid);
    void requestNextPage(ContentWidget *const w) const;
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
    void onDeviceStatusChanged(const dde::network::WirelessDevice::DeviceStatus stat);
    void disconnectWifi(const QString &uuid);

private Q_SLOTS:
    void sortAPList();
    void onApWidgetEditRequested(const QString &apPath, const QString &ssid);
    void onApWidgetConnectRequested(const QString &path, const QString &ssid);
    void showConnectHidePage();
    void onDeviceRemoved();
    void onActivateApFailed(const QString &apPath, const QString &uuid);
    void refreshLoadingIndicator();
    void onNetworkAdapterChanged(bool checked);

private:
    void updateActiveAp();
    QString connectionUuid(const QString &ssid);
    QString connectionSsid(const QString &uuid);
    void updateLayout(bool enabled);

private:
    dde::network::WirelessDevice *m_device;
    dde::network::NetworkModel *m_model;

    dcc::widgets::SwitchWidget *m_switch;
    dcc::widgets::SettingsGroup *m_tipsGroup;
    QPushButton *m_closeHotspotBtn;
    DListView *m_lvAP;
    APItem *m_clickedItem;
    QVBoxLayout *m_mainLayout;
    QStandardItemModel *m_modelAP;
    int m_layoutCount;
    WifiStatus m_preWifiStatus;
    QPointer<ConnectionWirelessEditPage> m_apEditPage;

    QString m_editingUuid;
    QString m_lastConnectSsid;
    QTimer *m_sortDelayTimer;
    QTimer *m_indicatorDelayTimer;
    QTimer *m_requestWirelessScanTimer;
    QMap<QString, APItem *> m_apItems;
    QString m_disconnectUuid;
};
}   // namespace dcc
}   // namespace network

Q_DECLARE_METATYPE(DCC_NAMESPACE::network::APSortInfo)

#endif // WIRELESSPAGE_H
