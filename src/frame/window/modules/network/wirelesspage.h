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
    bool connecting;
    //排序规则 ：1.是否处于连接状态;2.信号强度;3.字母大小排序
    bool operator <(const APSortInfo &other)
    {
        if (connected ^ other.connected) {
            return !connected;
        }
        if (connecting ^ other.connecting) {
            return !connecting;
        }
        if (signalstrength != other.signalstrength) {
            return signalstrength < other.signalstrength;
        }
        return ssid < other.ssid;
    }
};

class APItem :public QObject, public DStandardItem
{
   Q_OBJECT
public:
    explicit APItem(const QString &text, QStyle *style, DTK_WIDGET_NAMESPACE::DListView *parent = nullptr);
    virtual ~APItem();
    void setSecure(bool isSecure);
    bool secure() const;
    void setSignalStrength(int ss);
    int signalStrength() const;
    void setConnecting(bool connecting);
    bool isConnecting();
    void setConnected(bool connected);
    bool isConnected();
    void setSortInfo(const APSortInfo &si);
    APSortInfo sortInfo();
    void setPath(const QString &path);
    QString path() const;
    QString ssid() const;
    void setUuid(const QString &uuid);
    QString uuid() const;
    QAction *action() const;
    bool operator<(const QStandardItem &other) const override;
    /**
     * @def setLoading
     * @brief 是否显示连接中的转动图标
     * @param isLoading
     * @return 当前是否处于连接中状态
     */
    bool setLoading(bool isLoading);

Q_SIGNALS:
    /**
     * @def APChange信号
     * @brief 该信号会在wifi的信号强度改变的时候去发送，这样外部就无需再调用主动刷新，而可以直接连接槽函数进行被动刷新
     */
    void apChange();
    /**
     * @def  ActionChange
     * @brief m_arrowAction重新创建的时候发送，然后让WirelessPage去关联，达到重新关联信号和槽的目的
     */
    void ActionChange(APItem *item);

public:
    enum {
        SortRole = Dtk::UserRole + 1,
        PathRole,
        SecureRole
    };
private:
    DTK_WIDGET_NAMESPACE::DStyleHelper m_dStyleHelper;
    DViewItemAction *m_secureAction;
    bool m_preLoading;
    QString m_uuid;
    QPointer<DViewItemAction> m_loadingAction;
    /**
     * @variable  m_arrowAction
     * @brief 网络详情编辑页小箭头
     */
    QPointer<DViewItemAction> m_arrowAction;
    /**
     * @variable  m_loadingIndicator
     * @brief 连接动画小图标，由dtk提供
     */
    QPointer<DTK_WIDGET_NAMESPACE::DSpinner> m_loadingIndicator;
};

class WirelessPage : public dcc::ContentWidget
{
    Q_OBJECT
public:
    enum WifiStatus {
        Wifi_Unknown = 0,
        Wifi_Connecting,
        Wifi_Connected,
        Wifi_Disconnect
    };

    explicit WirelessPage(dde::network::WirelessDevice *dev, QWidget *parent = nullptr);
    virtual ~WirelessPage();

    void setModel(dde::network::NetworkModel *model);
    void jumpByUuid(const QString &uuid);
Q_SIGNALS:
    void requestNextPage(ContentWidget *const w) const;
    void requestDeviceRemanage(const QString &devPath) const;
    void requestDeviceAPList(const QString &devPath) const;
    void requestWirelessScan();
    void requestFrameKeepAutoHide(const bool autoHide) const;
    void requestShowAPEditPage(dde::network::NetworkDevice *device, const QString &session) const;
    void requestRemoveAPEditPage(dde::network::NetworkDevice *device) const;
    /**
     * @brief requestDisconnectConnection
     * @param uuid
     * @remark 发送断开连接的信号
     */
    void requestDisconnectConnection(const QString &uuid);
    /**
     * @brief requestDetailsPage
     * @param Widget
     * @remark 调用详情页
     */
    void requestDetailsPage(ContentWidget * const Widget) const;
    /**
     * @brief requestDeviceEnabled
     * @param devPath
     * @param enabled
     * @remark 开关信号发送
     */
    void requestDeviceEnabled(const QString &devPath, const bool enabled) const;
    /**
     * @brief UpdateWirelessScan
     * @remark 刷新wifi列表
     */
    void updateWirelessScan();
    /**
     * @brief requestShowApEditPage
     * @param device
     * @param session
     * @remark 请求打开wifi编辑页面
     */
    void requestShowApEditPage(dde::network::NetworkDevice *device, const QString &session) const;
    /**
     * @brief requestRemoveAp
     * @param uuid
     * @remark 删除某个wifi配置
     */
    void requestRemoveAp(const QString &uuid) const;


public Q_SLOTS:
    void onAPAdded(const QJsonObject &apInfo);
    void onAPChanged(const QJsonObject &apInfo);
    void onAPRemoved(const QJsonObject &apInfo);
    void onHotspotEnableChanged(const bool enabled);
    void onCloseHotspotClicked();
    /**
     * @brief onApConnect
     * @param apPath
     * @param uuid
     * @remark 请求连接wifi
     */
    void onActivaConnections(const QJsonObject &activeConn);
    /**
     * @brief onNetworkAdapterChanged
     * @param checked
     * @remark 网络适配器被修改
     */
    void onNetworkAdapterChanged(bool checked);
    /**
     * @def updateAction
     * @brief 刷新网络详情页小按钮和槽之间的连接
     * 由于DViewItemAction会在setAcionsList的时候释放旧的，所以会出现信号和槽断开的情况
     */
    void updateAction(APItem *item);

private Q_SLOTS:
    /**
     * @def sortAPList
     * @brief 对wifi进行排序
     */
    void sortAPList();
    /**
     * @def onApWidgetEditRequested
     * @brief 调用网络详情页
     * @param (const QString &apPath)
     * @param (const QString &ssid)
     */
    void onApWidgetEditRequested(const QString &apPath, const QString &ssid);
    void onApWidgetConnectRequested(const QString &path, const QString &ssid);
    void showConnectHidePage();
    void onDeviceRemoved();
    void onActivateApFailed(const QString &apPath, const QString &uuid);
    /**
     * @def onSwitchEnable
     * @brief 设置wifi开关变成禁用状态
     */
    void onSwitchEnable();
    /**
     * @def onSwitch
     * @brief 设置开关打开或者关闭
     * @param (bool enable)
     */
    void onSwitch(bool enable);
    void onClickApItem(const QModelIndex & idx);

private:

    QString connectionUuid(const QString &ssid);
    QString connectionSsid(const QString &uuid);
    void updateLayout(bool enabled);
    /**
     * @def canUpdateApList
     * @brief 判断是否更新wifi
     * @return
     */
    int canUpdateApList();
    /**
     * @def initUI
     * @brief 初始化UI页面
     */
    void initUI();
    /**
     * @def initConnect
     * @brief 初始化部分信号和槽
     */
    void initConnect();
  /**
   * @def ApLoadin
   * @brief wifi连接中的状态图标显示，如果要全部关闭的状态化，可以传个nullptr
   */
  bool setApLoadin(APItem *ApItem);
  /**
   * @def addHideWifiButton
   * @brief 创建连接隐藏wifi的选项，防止清理数据后消失
   */
  void addHideWifiButton();

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
    QPointer<ConnectionWirelessEditPage> m_apEditPage;

    QTimer *m_sortDelayTimer;
    //禁用wifi开关控件，防止用户对wifi开关疯狂操作
    QTimer *m_switchEnableTimer;
    QMap<QString, APItem *> m_apItems;
    QString m_disconnectUuid;

};
}   // namespace dcc
}   // namespace network

Q_DECLARE_METATYPE(DCC_NAMESPACE::network::APSortInfo)

#endif // WIRELESSPAGE_H
