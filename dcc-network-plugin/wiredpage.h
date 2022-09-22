// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WIREDPAGE_H
#define WIREDPAGE_H

#include "widgets/contentwidget.h"

#include <QPointer>

DWIDGET_BEGIN_NAMESPACE
class DFloatingButton;
class DListView;
class DStandardItem;
DWIDGET_END_NAMESPACE

namespace dcc {
  namespace widgets {
    class SwitchWidget;
    class SettingsGroup;
    class NextPageWidget;
  }
}

namespace dde {
  namespace network {
    class NetworkDeviceBase;
    class NetworkController;
    class WiredDevice;
    class WiredConnection;
    enum class DeviceStatus;
  }
}

class ConnectionEditPage;
class QStandardItemModel;

using namespace dde::network;
using namespace dcc::widgets;

DWIDGET_USE_NAMESPACE

class WiredPage : public dcc::ContentWidget
{
    Q_OBJECT

public:
    explicit WiredPage(dde::network::WiredDevice *dev, QWidget *parent = nullptr);
    ~ WiredPage();

    void jumpPath(const QString &searchPath);

Q_SIGNALS:
    void requestNextPage(ContentWidget * const w) const;
    void requestFrameKeepAutoHide(const bool autoHide) const;
    void requestDeviceEnabled(const QString &devPath, const bool enabled) const;

private Q_SLOTS:
    void refreshConnectionList();
    void editConnection(const QString &connectionPath);
    void createNewConnection();
    void activateEditConnection(const QString &connectPath, const QString &uuid);
    void checkActivatedConnection();
    void onDeviceStatusChanged(const DeviceStatus &stat);
    void onDeviceRemoved();
    void onUpdateConnectionStatus();
    void onConnectionPropertyChanged(const QList<WiredConnection *> &changedConnection);

private:
    dde::network::WiredDevice *m_device;
    NetworkController *m_pNetworkController;                    // 网络硬件接口
    SwitchWidget *m_switch;

    DListView *m_lvProfiles;
    QStandardItemModel *m_modelprofiles;

    QPointer<ConnectionEditPage> m_editPage;

    DFloatingButton *m_createBtn;
    SettingsGroup *m_tipsGrp;

    QMap<DStandardItem *, QString> m_connectionPath;
    QString m_newConnectionPath;

    static const int PathRole = Qt::UserRole + 100;
    static const int SortRole = Qt::UserRole + 101;
};

#endif // WIREDPAGE_H
