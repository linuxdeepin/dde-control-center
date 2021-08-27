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

#ifndef PPPOEPAGE_H
#define PPPOEPAGE_H

#include "widgets/contentwidget.h"
#include "interface/namespace.h"

#include <QPointer>

DWIDGET_BEGIN_NAMESPACE
class DListView;
class DSpinner;
class DFloatingButton;
class DViewItemAction;
DWIDGET_END_NAMESPACE

namespace dde {
  namespace network {
    class NetworkModel;
    class NetworkDeviceBase;
    class DSLItem;
  }
}

namespace dcc {
  namespace widgets {
    class SettingsGroup;
    class SwitchWidget;
    class LoadingNextPageWidget;
  }
}

DWIDGET_USE_NAMESPACE
using namespace dde::network;
using namespace dcc;

class ConnectionPageItem;
class ConnectionEditPage;
class QStandardItemModel;

class PppoePage : public ContentWidget
{
    Q_OBJECT
public:
    explicit PppoePage(QWidget *parent = nullptr);
    ~PppoePage();

    void jumpPath(const QString &searchPath);

Q_SIGNALS:
    void requestNextPage(ContentWidget *const w) const;
    void requestFrameKeepAutoHide(const bool autoHide) const;
    void requestActivateConnection(const QString &) const;
    void refreshConnectionList();

private Q_SLOTS:
    void createPPPoEConnection();
    void onConnectionListChanged();
    void onActiveConnectionChanged();

private:
    DListView *m_lvsettings;
    QStandardItemModel *m_modelSettings;

    DFloatingButton *m_createBtn;
    QPointer<ConnectionEditPage> m_editPage;
    QMap<QString, ConnectionPageItem *> m_items;

    static const int UuidRole = Qt::UserRole + 100;
    static const int PathRole = Qt::UserRole + 200;
};

#endif // PPPOEPAGE_H
