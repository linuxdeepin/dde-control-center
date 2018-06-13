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

#ifndef HOTSPOTPAGE_H
#define HOTSPOTPAGE_H

#include "contentwidget.h"
#include "connectioneditpage.h"

#include <QJsonObject>

namespace dde {
namespace network {
class WirelessDevice;
}
}

namespace dcc {

namespace widgets {

class NextPageWidget;
class SwitchWidget;

}

namespace network {

class HotspotPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit HotspotPage(dde::network::WirelessDevice *wdev, QWidget *parent = nullptr);

    void setModel(dde::network::NetworkModel *model);

signals:
    void requestNextPage(ContentWidget * const w) const;
    void requestNewHotspot(const QString &devPath) const;
    void requestDeviceRemanage(const QString &devPath) const;
    void requestDisconnectConnection(const QString &uuid) const;
    void requestDeleteConnection(const QString &uuid);
    void requestActivateConnection(const QString &devPath, const QString &uuid) const;
    void requestEditConnection(const QString &devPath, const QString &uuid) const;
    void requestFrameKeepAutoHide(const bool autoHide) const;

private:
    void onSwitchToggled(const bool checked);
    void onConfigWidgetClicked();
    void onConnectionsChanged();
    void onActiveConnsChanged();
    void onConnectionSessionCreated(const QString &sessionPath);

    void closeHotspot();
    void openHotspot();

    inline const QString hotspotUuid() const
    { return m_hotspotInfo.value("Uuid").toString(); }

private:
    dde::network::WirelessDevice * const m_wdev;
    dde::network::NetworkModel *m_model;
    widgets::SwitchWidget *m_hotspotSwitch;
    widgets::NextPageWidget *m_configureWidget;

    QJsonObject m_hotspotInfo;
    QPointer<ConnectionEditPage> m_editPage;
};

}

}

#endif // HOTSPOTPAGE_H
