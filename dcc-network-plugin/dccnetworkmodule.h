/*
 * Copyright (C) 2020 ~ 2021 Uniontech Technology Co., Ltd.
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

#ifndef NETWORKINTERFACE_H
#define NETWORKINTERFACE_H

#include <QObject>
#include "interface/namespace.h"
#include "interface/moduleinterface.h"
#include "interface/frameproxyinterface.h"

#include <com_deepin_daemon_airplanemode.h>

class NetworkModuleWidget;
class WirelessPage;
class ConnectionEditPage;

namespace dde {
  namespace network {
    class NetworkDeviceBase;
  }
}

namespace DCC_NAMESPACE {
  class ModuleInterface;
  class FrameProxyInterface;
}

enum class PageType;

using DBusAirplaneMode = com::deepin::daemon::AirplaneMode;

using namespace DCC_NAMESPACE;
using namespace dde::network;

class DCCNetworkModule : public QObject, public ModuleInterface
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID ModuleInterface_iid FILE "network.json")
    Q_INTERFACES(DCC_NAMESPACE::ModuleInterface)

signals:
    void deviceChanged();

public:
    explicit DCCNetworkModule();

    ~DCCNetworkModule() Q_DECL_OVERRIDE;

    void preInitialize(bool sync = false,FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) Q_DECL_OVERRIDE;
    // 初始化函数
    void initialize() Q_DECL_OVERRIDE;

    void active() Q_DECL_OVERRIDE;

    QStringList availPage() const Q_DECL_OVERRIDE;
    // 返回插件名称，用于显示
    const QString displayName() const Q_DECL_OVERRIDE;

    // 返回插件图标，用于显示
    QIcon icon() const Q_DECL_OVERRIDE;

    // 返回插件翻译文件路径，用于搜索跳转
    QString translationPath() const Q_DECL_OVERRIDE;

    // 告知控制中心插件级别（一级模块还是二级菜单），必须实现
    QString path() const Q_DECL_OVERRIDE;

    // 告知控制中心插件插入位置，必须实现
    QString follow() const Q_DECL_OVERRIDE;

    const QString name() const Q_DECL_OVERRIDE;

    void showPage(const QString &pageName) Q_DECL_OVERRIDE;

    QWidget *moduleWidget() Q_DECL_OVERRIDE;

    int load(const QString &path) Q_DECL_OVERRIDE;

    void addChildPageTrans() const Q_DECL_OVERRIDE;

    void initSearchData() Q_DECL_OVERRIDE;

private:
    void removeConnEditPageByDevice(NetworkDeviceBase *dev);
    void initListConfig();
    bool hasModule(const PageType &type);

private Q_SLOTS:
    void showWirelessEditPage(NetworkDeviceBase *dev, const QString &connUuid = QString(), const QString &apPath = QString());

    void showPppPage(const QString &searchPath);
    void showVPNPage(const QString &searchPath);
    void showDeviceDetailPage(NetworkDeviceBase *dev, const QString &searchPath = QString());
    void showChainsProxyPage();
    void showProxyPage();
    void showHotspotPage(const QString &searchPath);
    void showDetailPage();

private:
    NetworkModuleWidget *m_indexWidget;
    ConnectionEditPage *m_connEditPage;
    DBusAirplaneMode *m_airplaneMode;
};

#endif // NETWORKINTERFACE_H
