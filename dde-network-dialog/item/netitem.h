/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     donghualin <donghualin@uniontech.com>
 *
 * Maintainer: donghualin <donghualin@uniontech.com>
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

#ifndef NETITEM_H
#define NETITEM_H

#include <DStackedWidget>

#include <QAbstractListModel>
#include <QModelIndex>
#include <QJsonObject>
#include <QStyledItemDelegate>


class NetworkDevice;
class NetItem;
class WirelessConnect;
class QLabel;
class QPushButton;
class StateButton;

namespace dde {
    namespace network {
        class NetworkDeviceBase;
        class WiredDevice;
        class WirelessDevice;
        class AccessPoints;
        class WiredConnection;
        enum class DeviceType;
    } // namespace network
} // namespace dde

using namespace dde::network;
namespace Dtk {
    namespace Widget {
        class DListView;
        class DSwitchButton;
        class DViewItemAction;
        class DLoadingIndicator;
        class DSpinner;
        class DPasswordEdit;
        class DLineEdit;
        class DStandardItem;
    } // namespace Widget
} // namespace Dtk

DWIDGET_USE_NAMESPACE
//DGUI_USE_NAMESPACE


enum NetItemRole {
    TypeRole = Qt::UserRole + 100,
    DeviceDataRole,
    DataRole,
    ConnectionStatusRole,
    MouseInBoundingRole
};

#define PANELWIDTH 300

enum NetItemType {
    DeviceControllViewItem = 0, // 总控开关
    WirelessControllViewItem,   // 无线网卡开关
    WirelessViewItem,           // 无线列表
    WiredControllViewItem,      // 有线网卡开关
    WiredViewItem               // 有线列表
};

enum NetConnectionType {
    UnConnected = 0, // 未连接
    Connecting,      // 正在连接
    Connected        // 已连接
};

class NetItem : public QObject
{
    Q_OBJECT

public:
    NetItem(QWidget *parent);
    virtual ~NetItem();

    virtual DStandardItem *standardItem();
    virtual void updateView();
    virtual NetItemType itemType() = 0;

private:
    DStandardItem *m_standardItem;
    QWidget *m_parentWidget;
};

class DeviceControllItem : public NetItem
{
    Q_OBJECT

public:
    DeviceControllItem(const DeviceType &deviceType, QWidget *parent);
    ~DeviceControllItem() Q_DECL_OVERRIDE;

    void setDevices(const QList<NetworkDeviceBase *> &devices);
    DeviceType deviceType();
    void updateView() Q_DECL_OVERRIDE;
    NetItemType itemType() Q_DECL_OVERRIDE;

private:
    void initItemText();
    void initSwitcher();
    void initConnection();

private Q_SLOTS:
    void onSwitchDevices(bool on);

private:
    QList<NetworkDeviceBase *> m_devices;
    DeviceType m_deviceType;
    DSwitchButton *m_switcher;
};

class WiredControllItem : public NetItem
{
    Q_OBJECT

public:
    WiredControllItem(QWidget *parent, WiredDevice *device);
    ~WiredControllItem() Q_DECL_OVERRIDE;

    WiredDevice *device();
    void updateView() Q_DECL_OVERRIDE;
    NetItemType itemType() Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onSwitchDevices(bool on);

private:
    WiredDevice *m_device;
    DSwitchButton *m_switcher;
};

class WirelessControllItem : public NetItem
{
    Q_OBJECT

public:
    WirelessControllItem(QWidget *parent, WirelessDevice *device);
    ~WirelessControllItem() Q_DECL_OVERRIDE;

    WirelessDevice *device();
    void updateView() Q_DECL_OVERRIDE;
    NetItemType itemType() Q_DECL_OVERRIDE;

protected:
    bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;

    QString iconFile();

private Q_SLOTS:
    void onSwitchDevices(bool on);

private:
    WirelessDevice *m_device;
    QWidget *m_widget;
    DSwitchButton *m_switcher;
    DLoadingIndicator *m_loadingIndicator;
};

class WiredItem : public NetItem
{
    Q_OBJECT

public:
    WiredItem(QWidget *parent, WiredDevice *device, WiredConnection *connection);
    ~WiredItem() Q_DECL_OVERRIDE;

    WiredConnection *connection();
    void updateView() Q_DECL_OVERRIDE;
    NetItemType itemType() Q_DECL_OVERRIDE;
    void connectNetwork();

private:
    void initUi();
    QString symbolicIcon(const bool &connected) const;
    void initConnection();

private:
    WiredConnection *m_connection;
    WiredDevice *m_device;
    DViewItemAction *m_connectionIconAction;
};

class WirelessItem : public NetItem
{
    Q_OBJECT

Q_SIGNALS:
    void sizeChanged();

public:
    enum ExpandWidget {
        Hide = -1,
        ShowPassword,
        ShowSSID,
    };

public:
    WirelessItem(QWidget *parent, WirelessDevice *device, AccessPoints *ap);
    ~WirelessItem() Q_DECL_OVERRIDE;

    const AccessPoints *accessPoint();
    const WirelessDevice *wirelessDevice();
    void updateView() Q_DECL_OVERRIDE;
    NetItemType itemType() Q_DECL_OVERRIDE;
    static QString getStrengthStateString(int strength);
    void expandWidget(ExpandWidget type);
    void connectNetwork();
    void expandPasswordInput();

private:
    void initUi(QWidget *parent);
    void initConnection();
    void updateSrcirityIcon();
    void updateWifiIcon();
    void updateConnectionStatus();
    void createPasswordEdit();
    void createSsidEdit();
    void initExpandUi();

private Q_SLOTS:
    void onConnection();
    void onConnectNetwork();
    void onInputPassword(const QString oldPassword);
    void onConnectHidden();
    void checkInputValid();

private:
    AccessPoints *m_accessPoint;
    WirelessDevice *m_device;
    DViewItemAction *m_securityAction;
    DViewItemAction *m_wifiLabel;
    DViewItemAction *m_connectionAction;
    DSpinner *m_loadingStat;

    QWidget *m_connectionWidget;
    StateButton *m_connIcon;

    DStackedWidget *m_stackWidget;
    DViewItemAction *m_expandItem;

    DPasswordEdit *m_passwdEdit;
    DLineEdit *m_ssidEdit;

    WirelessConnect *m_wirelessConnect;
    DPushButton *m_connectButton;
};

#endif //  NETWORKAPPLETMODEL_H
