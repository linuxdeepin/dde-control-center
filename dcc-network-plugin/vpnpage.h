/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
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
private:
    SwitchWidget *m_vpnSwitch;

    QPointer<ConnectionVpnEditPage> m_editPage;
    QString m_editingConnUuid;

    DListView *m_lvprofiles;
    QStandardItemModel *m_modelprofiles;
    QFileDialog *m_importFile;
    static const int UuidRole = Qt::UserRole + 100;
};

#endif // VPNPAGE_H
