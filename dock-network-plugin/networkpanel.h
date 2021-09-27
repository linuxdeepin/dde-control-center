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

#ifndef NETWORKPANEL_H
#define NETWORKPANEL_H

#include "item/devicestatushandler.h"

#include <DGuiApplicationHelper>
#include <DSwitchButton>
#include <DListView>
#include <DStyledItemDelegate>
#include <dloadingindicator.h>

#include <QWidget>
#include <QScopedPointer>

#include <com_deepin_daemon_network.h>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

namespace Dock {
  class TipsWidget;
}

namespace dde {
  namespace network {
    enum class DeviceType;
    class NetworkDeviceBase;
  }
}

class QTimer;
class NetItem;

using namespace dde::network;
using DbusNetwork = com::deepin::daemon::Network;

class NetworkPanel : public QWidget
{
    Q_OBJECT

Q_SIGNALS:
    void sendIpConflictDect(int);

public:
    explicit NetworkPanel(QWidget *parent = Q_NULLPTR);
    ~NetworkPanel();

    void invokeMenuItem(const QString &menuId);
    bool needShowControlCenter();
    const QString contextMenu() const;
    QWidget *itemTips();
    QWidget *itemApplet();
    bool hasDevice();

    void refreshIcon();

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void setControlBackground();
    void initUi();
    void initConnection();
    void getPluginState();
    void updateView();                                                  // 更新网络列表内容大小
    void updateTooltips();                                              // 更新提示的内容
    void updateItems();
    bool deviceEnabled(const DeviceType &deviceType) const;
    void setDeviceEnabled(const DeviceType &deviceType, bool enabeld);

    int getStrongestAp();
    int deviceCount(const DeviceType &devType);
    QStringList ipTipsMessage(const DeviceType &devType);

    void enterEvent(QEvent *event);

    QStringList getIPList(const DeviceType &deviceType) const;
    QStringList getActiveWiredList() const;
    QStringList getActiveWirelessList() const;
    QStringList currentIpList() const;

private Q_SLOTS:
    void onDeviceAdded(QList<NetworkDeviceBase *> devices);

    void onUpdatePlugView();

    void onClickListView(const QModelIndex &index);
    void onIPConfllict(const QString &ip, const QString &mac);
    void onSendIpConflictDect(int index = 0);
    void onDetectConflict();

private:
    PluginState m_pluginState;

    QTimer *m_refreshIconTimer;
    QTimer *m_switchWireTimer;
    QTimer *m_wirelessScanTimer;

    Dock::TipsWidget *m_tipsWidget;
    bool m_switchWire;
    QPixmap m_iconPixmap;

    QStandardItemModel *m_model;

    QScrollArea *m_applet;
    QWidget *m_centerWidget;
    DListView *m_netListView;
    // 判断定时的时间是否到,否则不重置计时器
    bool m_timeOut;

    QList<NetItem *> m_items;

    DbusNetwork *m_networkInter;
    QStringList m_disconflictList;         // 解除冲突数据列表
    QMap<QString, QString> m_conflictMap;  // 缓存有线和无线冲突的ip列表
    QTimer *m_detectConflictTimer;         // 定时器自检,当其他主机主动解除ip冲突，我方需要更新网络状态
    bool m_ipConflict;                     // ip冲突的标识
    bool m_ipConflictChecking;             // 标记是否正在检测中
};

class NetworkDelegate : public DStyledItemDelegate
{
    Q_OBJECT

public:
    NetworkDelegate(QAbstractItemView *parent = Q_NULLPTR);
    ~NetworkDelegate() Q_DECL_OVERRIDE;

private:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

    bool needDrawLine(const QModelIndex &index) const;
};

#endif // NETWORKPANEL_H
