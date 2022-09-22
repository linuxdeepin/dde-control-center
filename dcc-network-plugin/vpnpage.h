// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNPAGE_H
#define VPNPAGE_H

#include "interface/namespace.h"

#include <DListView>

#include <QPointer>

class QFileDialog;

namespace dcc {
  class ContentWidget;
  namespace widgets {
    class SettingsGroup;
    class SwitchWidget;
    class LoadingNextPageWidget;
  }
}

namespace dde {
  namespace network {
    class VPNItem;
  }
}

using namespace dde::network;
using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE;

class ConnectionVpnEditPage;
class VpnPage : public QWidget
{
    Q_OBJECT

public:
    explicit VpnPage(QWidget *parent = nullptr);
    ~VpnPage();
    void jumpPath(const QString &searchPath);

Q_SIGNALS:
    void requestNextPage(dcc::ContentWidget *const w) const;
    void requestVpnEnabled(const bool enabled) const;
    void requestFrameKeepAutoHide(const bool autoHide) const;

private Q_SLOTS:
    void refreshVpnList(QList<VPNItem *> vpns);
    void updateVpnItems(const QList<VPNItem *> &vpns);

    void onActiveConnsInfoChanged();

    void importVPN();
    void createVPN();
    void changeVpnId();
    void showEditPage(VPNItem *vpn);

private:
    SwitchWidget *m_vpnSwitch;

    QPointer<ConnectionVpnEditPage> m_editPage;
    QString m_editingConnUuid;

    DListView *m_lvprofiles;
    QStandardItemModel *m_modelprofiles;
    QFileDialog *m_importFile;
    QString m_newConnectionPath;

    static const int UuidRole = Qt::UserRole + 100;
};

#endif // VPNPAGE_H
