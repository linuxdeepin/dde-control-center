// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NETWORKPLUGINHELPER_H
#define NETWORKPLUGINHELPER_H
#include "utils.h"
#include "item/devicestatushandler.h"

#include <DGuiApplicationHelper>

#include <NetworkManagerQt/Device>

#include <com_deepin_daemon_network.h>

DGUI_USE_NAMESPACE

namespace dde {
namespace network {
enum class DeviceType;
class NetworkDeviceBase;
class AccessPoints;
} // namespace network
} // namespace dde

class QTimer;
class NetItem;

using namespace dde::network;

namespace dde {
namespace networkplugin {

class NetworkDialog;
class TipsWidget;

class NetworkPluginHelper : public QObject
{
    Q_OBJECT

Q_SIGNALS:
    void sendIpConflictDect(int);
    void addDevice(const QString &devicePath);
    void viewUpdate();

public:
    explicit NetworkPluginHelper(NetworkDialog *networkDialog, QObject *parent = Q_NULLPTR);
    ~NetworkPluginHelper();

    void invokeMenuItem(const QString &menuId);
    bool needShowControlCenter();
    const QString contextMenu(bool hasSetting) const;
    QWidget *itemTips();
    PluginState getPluginState();
    void updateTooltips(); // 更新提示的内容

private:
    void initUi();
    void initConnection();
    bool deviceEnabled(const DeviceType &deviceType) const;
    void setDeviceEnabled(const DeviceType &deviceType, bool enabeld);
    bool wirelessIsActive() const;
    void handleAccessPointSecure(AccessPoints *accessPoint);

    int deviceCount(const DeviceType &devType) const;
    QList<QPair<QString, QStringList>> ipTipsMessage(const DeviceType &devType);
    bool needSetPassword(AccessPoints *accessPoint) const;

private Q_SLOTS:
    void onDeviceAdded(QList<NetworkDeviceBase *> devices);
    void onUpdatePlugView();
    void onActiveConnectionChanged();

    void onAccessPointsAdded(QList<AccessPoints *> newAps);

private:
    TipsWidget *m_tipsWidget;
    bool m_switchWire;
    QPixmap m_iconPixmap;

    QSet<QString> m_devicePaths; // 记录无线设备Path,防止信号重复连接
    QString m_lastActiveWirelessDevicePath;
    NetworkDialog *m_networkDialog;
};

}
}

#endif // NETWORKPLUGINHELPER_H
