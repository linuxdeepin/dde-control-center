// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HOTSPOTPAGE_H
#define HOTSPOTPAGE_H

#include "widgets/contentwidget.h"
#include "interface/namespace.h"

#include <DListView>
#include <DFloatingButton>

#include <QPointer>

namespace dde {
  namespace network {
    class WirelessDevice;
    class AccessPoints;
    class HotspotItem;
  }
}

namespace dcc {
  namespace widgets {
    class NextPageWidget;
    class SwitchWidget;
  }
}

using namespace dde::network;
using namespace dcc::widgets;

class HotspotPage;
class ConnectionHotspotEditPage;

class HotspotDeviceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HotspotDeviceWidget(WirelessDevice *wdev, QWidget *parent = nullptr);
    virtual ~HotspotDeviceWidget();
    void setPage(HotspotPage *p);

    inline WirelessDevice *device() { return m_device; }

    void addItems(const QList<HotspotItem *> &newItems);
    void removeItems(const QList<HotspotItem *> &rmItems);
    void updateItemStatus(const QList<HotspotItem *> &items);
    void updateCreateButtonStatus(bool showcreatebtn);

private:
    void closeHotspot();
    void openHotspot();
    void openEditPage(const QString &uuid = QString());

Q_SIGNALS:
    void requestDisconnectConnection(const QString &uuid) const;

private Q_SLOTS:
    void onDeviceRemoved();
    void onSwitchToggled(const bool checked);
    void onConnWidgetSelected(const QModelIndex &idx);
    void onConnEditRequested(const QString &uuid);
    void onHotsportEnabledChanged();

private:
    WirelessDevice * const m_device;
    SwitchWidget *m_hotspotSwitch;
    DListView *m_lvprofiles;
    QStandardItemModel *m_modelprofiles;

    QPushButton *m_createBtn;
    QPointer<ConnectionHotspotEditPage> m_editPage;

    HotspotPage *m_page;
    bool m_isClicked;

    static const int UuidRole = Dtk::UserRole + 1;
    static const int itemRole = Dtk::UserRole + 2;

    friend class HotspotPage;

    const QList<HotspotItem *> m_items;
};

class HotspotPage : public QWidget
{
    Q_OBJECT

public:
    explicit HotspotPage(QWidget *parent = nullptr);
    ~ HotspotPage();
    void jumpPath(const QString &path);

Q_SIGNALS:
    void requestNextPage(dcc::ContentWidget * const w) const;

public Q_SLOTS:
    void onAirplaneModeChanged(bool airplaneModeEnabled);

private Q_SLOTS:
    void onDeviceAdded(const QList<WirelessDevice *> &devices);
    void onDeviceRemove(const QList<WirelessDevice *> &rmDevices);
    void onItemAdded(const QMap<WirelessDevice *, QList<HotspotItem *>> &deviceItems);
    void onItemRemoved(const QMap<WirelessDevice *, QList<HotspotItem *>> &deviceItems);
    void onItemChanged(const QMap<WirelessDevice *, QList<HotspotItem *>> &deviceItems);
    void onActiveConnectionChanged(const QList<WirelessDevice *> &devices);

private:
    QWidget *m_contents;
    QVBoxLayout *m_vScrollLayout;
    QVBoxLayout *m_mainLayout;
    QList<HotspotDeviceWidget *> m_listdevw;
    DFloatingButton *m_newprofile;
};

#endif // HOTSPOTPAGE_H
