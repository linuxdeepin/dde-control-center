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

#include "interfaces/moduleinterface.h"
#include "connectioneditpage.h"

namespace dde {
namespace network {
class WirelessDevice;
class NetworkDevice;
class NetworkWorker;
class NetworkModel;
}
}

namespace dcc {
namespace network {

class NetworkModuleWidget;
class NetworkModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit NetworkModule(FrameProxyInterface *frame, QObject *parent = 0);
    ~NetworkModule();
    void showPage(const QString &jsonData);

private:
    void initialize();
    void reset();
    void moduleActive();
    void moduleDeactive();
    void contentPopped(ContentWidget * const w);
    const QString name() const;
    ModuleWidget *moduleWidget();

private Q_SLOTS:
    void showVpnPage();
    void showPppPage();
    void showChainsProxyPage();
    void showProxyPage();
    void showChainsProxyTypePage();
    void showDetailPage();
    void showHotspotPage(dde::network::WirelessDevice *wdev);
    void showDeviceDetailPage(dde::network::NetworkDevice *dev);
    void onSetFrameAutoHide(const bool autoHide);

    void showWiredEditPage(dde::network::NetworkDevice *dev, const QString &connUuid = QString());
    void showWirelessEditPage(dde::network::NetworkDevice *dev, const QString &connUuid = QString(), const QString &apPath = QString());
    void removeConnEditPageByDevice(dde::network::NetworkDevice *dev);

private:
    dde::network::NetworkModel *m_networkModel;
    dde::network::NetworkWorker *m_networkWorker;
    NetworkModuleWidget *m_networkWidget;

    ConnectionEditPage *m_connEditPage;
};

} // namespace network
} // namespace dcc

#endif // NETWORKMODULE_H
