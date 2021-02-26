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

#include "widgets/contentwidget.h"
#include "interface/namespace.h"

#include <DListView>
#include <DFloatingButton>

#include <QPointer>

namespace dde {
namespace network {
class NetworkModel;
class NetworkDevice;
class WirelessDevice;
}
}

namespace dcc {
namespace widgets {
class NextPageWidget;
class SwitchWidget;
}
}

namespace DCC_NAMESPACE {
namespace network {
class HotspotPage;
class ConnectionHotspotEditPage;
class HotspotDeviceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HotspotDeviceWidget(dde::network::WirelessDevice *wdev, bool showcreatebtn = true, QWidget *parent = nullptr);
    virtual ~HotspotDeviceWidget();
    void setModel(dde::network::NetworkModel *model);
    void setPage(HotspotPage *p);

private:
    void closeHotspot();
    void openHotspot();
    void openEditPage(const QString &uuid = QString());

Q_SIGNALS:
    void requestDeviceRemanage(const QString &devPath) const;
    void requestDisconnectConnection(const QString &uuid) const;

private Q_SLOTS:
    void onDeviceRemoved();
    void onSwitchToggled(const bool checked);
    void onConnWidgetSelected(const QModelIndex &idx);
    void onConnEditRequested(const QString &uuid);
    void onHotsportEnabledChanged();
    void refreshHotspotConnectionList();
    void refreshActiveConnection();

private:
    dde::network::WirelessDevice * const m_wdev;
    dde::network::NetworkModel *m_model;
    dcc::widgets::SwitchWidget *m_hotspotSwitch;
    DListView *m_lvprofiles;
    QStandardItemModel *m_modelprofiles;

    QPushButton *m_createBtn;
    QPointer<ConnectionHotspotEditPage> m_editPage;

    QTimer *m_refreshActiveTimer;

    HotspotPage *m_page;

    static const int UuidRole = Dtk::UserRole + 1;

    friend class HotspotPage;
    bool m_isClicked;
};

class HotspotPage : public QWidget
{
    Q_OBJECT
public:
    explicit HotspotPage(QWidget *parent = nullptr);
    ~ HotspotPage();
    void setModel(dde::network::NetworkModel *model);

Q_SIGNALS:
    void back();
    void requestNextPage(dcc::ContentWidget * const w) const;
    void requestDeviceRemanage(const QString &devPath) const;
    void requestDisconnectConnection(const QString &uuid) const;
    void requestActivateConnection(const QString &devPath, const QString &uuid) const;

private Q_SLOTS:
    void deviceListChanged(const QList<dde::network::NetworkDevice *> &devices);

private:
    dde::network::NetworkModel *m_model;
    QWidget *m_contents;
    QVBoxLayout *m_vScrollLayout;
    QVBoxLayout *m_mainLayout;
    QList<HotspotDeviceWidget *> m_listdevw;
    DTK_WIDGET_NAMESPACE::DFloatingButton *m_newprofile;
};
}

}

#endif // HOTSPOTPAGE_H
