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

#include "connectionhotspoteditpage.h"
#include "widgets/contentwidget.h"

#include <QPointer>
#include <QJsonObject>

namespace dde {
namespace network {
class NetworkModel;
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

Q_SIGNALS:
    void requestNextPage(ContentWidget * const w) const;
    void requestDeviceRemanage(const QString &devPath) const;
    void requestDisconnectConnection(const QString &uuid) const;
    void requestActivateConnection(const QString &devPath, const QString &uuid) const;
    void requestFrameKeepAutoHide(const bool autoHide) const;

private:
    void closeHotspot();
    void openHotspot();
    void openEditPage(const QString &uuid = QString());
    QString uuidByConnWidget(QObject *connWidget);

private Q_SLOTS:
    void onDeviceRemoved();
    void onSwitchToggled(const bool checked);
    void onConnWidgetSelected();
    void onConnWidgetNextPage();
    void onHotsportEnabledChanged();
    void refreshHotspotConnectionList();
    void refreshActiveConnection();

private:
    dde::network::WirelessDevice * const m_wdev;
    dde::network::NetworkModel *m_model;
    widgets::SwitchWidget *m_hotspotSwitch;
    widgets::SettingsGroup *m_connectionsGroup;

    QPushButton *m_createBtn;
    QPointer<ConnectionHotspotEditPage> m_editPage;

    QTimer *m_refreshActiveTimer;

    QMap<widgets::NextPageWidget *, QString> m_connWidgetUuidMap;
};

}

}

#endif // HOTSPOTPAGE_H
