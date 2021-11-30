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

#include <NetworkManagerQt/Device>

#include <com_deepin_daemon_network.h>

DGUI_USE_NAMESPACE

class TipsWidget;

namespace dde {
namespace network {
enum class DeviceType;
class NetworkDeviceBase;
} // namespace network
} // namespace dde

class NetworkDialog;
class QTimer;
class NetItem;

using namespace dde::network;

class NetworkPanel : public QObject
{
    Q_OBJECT

Q_SIGNALS:
    void sendIpConflictDect(int);
    void addDevice(const QString &devicePath);
    void viewUpdate();

public:
    explicit NetworkPanel(QObject *parent = Q_NULLPTR);
    ~NetworkPanel();

    void setMainWidget(QWidget *mainWidget);

    void invokeMenuItem(const QString &menuId);
    bool needShowControlCenter();
    const QString contextMenu(bool hasSetting) const;
    QWidget *itemTips();
    PluginState getPluginState();
    void updatePluginState();

private:
    void initUi();
    void initConnection();
    void updateTooltips(); // 更新提示的内容
    bool deviceEnabled(const DeviceType &deviceType) const;
    void setDeviceEnabled(const DeviceType &deviceType, bool enabeld);

    int deviceCount(const DeviceType &devType) const;
    QList<QPair<QString, QStringList>> ipTipsMessage(const DeviceType &devType);

private Q_SLOTS:
    void onDeviceAdded(QList<NetworkDeviceBase *> devices);
    void onUpdatePlugView();

private:
    PluginState m_pluginState;

    QTimer *m_wirelessScanTimer;

    TipsWidget *m_tipsWidget;
    bool m_switchWire;
    QPixmap m_iconPixmap;

    QSet<QString> m_devicePaths; // 记录无线设备Path,防止信号重复连接
    QString m_lastActiveWirelessDevicePath;
    QWidget *m_mainWidget;
};

#endif // NETWORKPANEL_H
