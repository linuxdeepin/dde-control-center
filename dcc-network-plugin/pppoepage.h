// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
    void refreshConnectionList();

private Q_SLOTS:
    void createPPPoEConnection();
    void onConnectionListChanged();
    void onItemChanged(const QList<DSLItem *> &items);
    void onActiveConnectionChanged();
    void onShowEditPage(const QString &devicePath, const QString &uuid);

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
