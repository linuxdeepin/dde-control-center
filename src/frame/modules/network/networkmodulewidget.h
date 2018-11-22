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

#ifndef NETWORKMODULEWIDGET_H
#define NETWORKMODULEWIDGET_H

#include "modules/modulewidget.h"
#include "widgets/nextpagewidget.h"

namespace dde {
namespace network {
class NetworkDevice;
class WirelessDevice;
class NetworkModel;
}
}

namespace dcc{
namespace widgets {
class SettingsGroup;
}

namespace network {

class NetworkModuleWidget : public dcc::ModuleWidget
{
    Q_OBJECT

public:
    explicit NetworkModuleWidget();

    void setModel(dde::network::NetworkModel *model);

Q_SIGNALS:
    void requestShowVpnPage() const;
    void requestShowPppPage() const;
    void requestShowProxyPage() const;
    void requestShowInfomation() const;
    void requestShowChainsPage() const;
    void requestHotspotPage(dde::network::WirelessDevice *wdev) const;
    void requestShowDeviceDetail(dde::network::NetworkDevice *device) const;
    void requestDeviceEnable(const QString &devPath, const bool enable) const;

private Q_SLOTS:
    void onDeviceListChanged(const QList<dde::network::NetworkDevice *> &devices);
    void onDevStatusChanged();
    void onNextPageClicked();

private:
    void createDeviceGroup(dde::network::NetworkDevice *dev, const int number, const bool multiple);

private:
    QVBoxLayout *m_devicesLayout;
    dcc::widgets::NextPageWidget *m_pppBtn;
    dcc::widgets::NextPageWidget *m_vpnBtn;
    dcc::widgets::NextPageWidget *m_proxyBtn;
    dcc::widgets::NextPageWidget *m_detailBtn;
    dcc::widgets::NextPageWidget *m_appProxy;

    QMap<dcc::widgets::NextPageWidget *, dde::network::NetworkDevice *> m_devicesWidgets;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKMODULEWIDGET_H
