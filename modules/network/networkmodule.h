/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#ifndef NETWORKMODULE_H
#define NETWORKMODULE_H

#include "moduleinterface.h"

namespace dcc {
namespace network {

class WirelessDevice;
class NetworkDevice;
class NetworkModuleWidget;
class NetworkWorker;
class NetworkModel;
class NetworkModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit NetworkModule(FrameProxyInterface *frame, QObject *parent = 0);
    ~NetworkModule();
    void showPage(const QString &pageName);

private:
    void initialize();
    void reset();
    void moduleActive();
    void moduleDeactive();
    void contentPopped(ContentWidget * const w);
    const QString name() const;
    ModuleWidget *moduleWidget();

private slots:
    void showVpnPage();
    void showPppPage();
    void showChainsProxyPage();
    void showProxyPage();
    void showChainsProxyTypePage();
    void showDetailPage();
    void showHotspotPage(WirelessDevice *wdev);
    void showDeviceDetailPage(NetworkDevice *dev);
    void showWiredConnectionEditPage(const QString &session);
    void onSetFrameAutoHide(const bool autoHide);

private:
    NetworkModel *m_networkModel;
    NetworkWorker *m_networkWorker;
    NetworkModuleWidget *m_networkWidget;

    QString m_editingWiredUuid;
};

} // namespace network
} // namespace dcc

#endif // NETWORKMODULE_H
