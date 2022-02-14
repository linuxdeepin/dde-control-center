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

#include <NetworkManagerQt/Device>

#include <QWindow>

#include <DListView>
#include <DStyledItemDelegate>

#include <com_deepin_daemon_airplanemode.h>

namespace dde {
  namespace network {
    enum class DeviceType;
    class NetworkDeviceBase;
  }
}

class NetItem;
class QStandardItemModel;
class QTimer;
class QScrollArea;

using DBusAirplaneMode = com::deepin::daemon::AirplaneMode;
using namespace dde::network;

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

class NetworkPanel : public QObject
{
    Q_OBJECT

public:
    explicit NetworkPanel(QObject *parent = Q_NULLPTR);
    ~NetworkPanel() override;

    QWidget *itemApplet();
    void passwordError(const QString &dev, const QString &ssid);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setControlBackground();
    void initUi();
    void updateItems();
    bool deviceEnabled(const DeviceType &deviceType) const;
    void setDeviceEnabled(const DeviceType &deviceType, bool enabeld);

    int getStrongestAp();
    int deviceCount(const DeviceType &devType);
    void expandPasswordInput();
    NetItem *selectItem();

private Q_SLOTS:
    void initConnection();
    void onDeviceAdded(QList<NetworkDeviceBase *> devices);
    void onUpdatePlugView();
    void onEnabledClicked(const QModelIndex &index, const bool enabled);
    void onRefreshClicked(const QModelIndex &index);

    void onClickListView(const QModelIndex &index);
    void updateView(); // 更新网络列表内容大小
    void refreshItems();

private:
    QTimer *m_wirelessScanTimer;

    bool m_switchWire;

    QStandardItemModel *m_model;

    QScrollArea *m_applet;
    QWidget *m_centerWidget;
    DListView *m_netListView;

    QList<NetItem *> m_items;
    // 记录无线设备Path,防止信号重复连接
    QSet<QString> m_wirelessDevicePath;
    QString m_reconnectDev;
    QString m_reconnectSsid;
    DBusAirplaneMode *m_airplaneMode;
    QTimer *m_updateTimer;
};

class NetworkDelegate : public DStyledItemDelegate
{
    Q_OBJECT

public:
    explicit NetworkDelegate(QAbstractItemView *parent = Q_NULLPTR);
    ~NetworkDelegate() Q_DECL_OVERRIDE;
    void setDBusAirplaneMode(DBusAirplaneMode *airplane);

Q_SIGNALS:
    void refreshClicked(const QModelIndex &);
    void closeClicked(const QModelIndex &);
    void enabledClicked(const QModelIndex &, const bool);

protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

    bool needDrawLine(const QModelIndex &index) const;
    bool cantHover(const QModelIndex &index) const;
    bool hasSwitchButton(const QModelIndex &index) const;

    void drawCheck(QPainter *painter, QRect &rect, QPen &pen, int radius) const;
    void drawFork(QPainter *painter, QRect &rect, QPen &pen, int radius) const;
    void drawLoading(QPainter *painter, QRect &rect, int diameter) const;
    void drawRefreshButton(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawSwitchButton(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

    QRect checkRect(const QRect &rct) const;

    QList<QColor> createDefaultIndicatorColorList(QColor color) const;

private:
    bool switchIsEnabled(const QModelIndex &index) const;

private:
    QAbstractItemView *m_parentWidget;
    QTimer *m_refreshIconTimer;
    mutable double m_currentDegree;
    QTimer *m_refreshTimer;
    DBusAirplaneMode *m_airplaneMode;
    mutable QList<QModelIndex> m_ConnectioningIndexs;
    mutable QMap<QModelIndex, int> m_refreshAngle;
};

#endif // NETWORKPANEL_H
