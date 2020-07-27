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

#ifndef NETWORKMODULE_H
#define NETWORKMODULE_H

#include "interface/moduleinterface.h"
#include "interface/namespace.h"

namespace dde {
namespace network {
class WirelessDevice;
class NetworkDevice;
class NetworkWorker;
class NetworkModel;
}
}

namespace DCC_NAMESPACE {
namespace network {
class ConnectionEditPage;
class NetworkModuleWidget;
class WirelessPage;
class NetworkModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit NetworkModule(FrameProxyInterface *frame, QObject *parent = nullptr);
    ~NetworkModule();
    void showPage(const QString &jsonData) override;

private:
    void preInitialize(bool sync = false) override;
    void initialize() override;
    const QString name() const override;
    const QString displayName() const override;
public:
    void active() override;
    int load(QString path) override;
    QStringList availPage() const override;

private Q_SLOTS:
    void popPage();
    void showVpnPage(const QString &searchPath = "");
    void showPppPage(const QString &searchPath = "");
    void showChainsProxyPage();
    void showProxyPage();
    void showDetailPage();
    void showHotspotPage();
    void ShowAirplanePage();
    void showDeviceDetailPage(dde::network::NetworkDevice *dev, const QString &searchPath = "");
    void onSetFrameAutoHide(const bool autoHide);
    void onDeviceListChanged(const QList<dde::network::NetworkDevice *> devices);
    void showWiredEditPage(dde::network::NetworkDevice *dev, const QString &connUuid = QString());
    void showWirelessEditPage(dde::network::NetworkDevice *dev, const QString &connUuid = QString(), const QString &apPath = QString());
    void removeConnEditPageByDevice(dde::network::NetworkDevice *dev);

private:
    dde::network::NetworkModel *m_networkModel;
    dde::network::NetworkWorker *m_networkWorker;
    QPointer<WirelessPage> m_wirelessPage;
    NetworkModuleWidget *m_networkWidget;
    ConnectionEditPage *m_connEditPage;
    QTimer *m_initSettingTimer;

    bool m_hasAp = false;
    bool m_hasWired = false;
    bool m_hasWireless = false;
};
} // namespace network
} // namespace dcc

#endif // NETWORKMODULE_H
