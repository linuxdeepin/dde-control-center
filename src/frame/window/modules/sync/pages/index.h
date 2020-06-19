/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     justforlxz <justforlxz@outlook.com>
 *
 * Maintainer: justforlxz <justforlxz@outlook.com>
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
#pragma once

#include "interface/namespace.h"
#include "modules/sync/utils.h"
#include "loginedin.h"

#include <dtkwidget_global.h>

#include <QWidget>

DWIDGET_BEGIN_NAMESPACE
class DListView;
class DTipLabel;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QListView;
class QVBoxLayout;
class QStandardItemModel;
class QStandardItem;
class QLabel;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class SwitchWidget;
}

namespace cloudsync {
class SyncModel;
class SyncStateIcon;
}
}

namespace DCC_NAMESPACE {
namespace sync {
class DownloadUrl;
class AvatarWidget;
class IndexPage : public LoginedIn
{
    Q_OBJECT
public:
    IndexPage(QWidget *parent = nullptr);
    void setModel(dcc::cloudsync::SyncModel *model);
    ~IndexPage();

Q_SIGNALS:
    void requestSetAutoSync(bool enable) const;
    void requestLogout() const;
    void requestSetModuleState(std::pair<dcc::cloudsync::SyncType, bool> state);

private:
    void onListViewClicked(const QModelIndex &index);
    void onStateChanged(const std::pair<qint32, QString> &state);
    void onLastSyncTimeChanged(const qlonglong lastSyncTime);
    void onModuleStateChanged(std::pair<dcc::cloudsync::SyncType, bool> state);
    void onModuleItemSwitched(const bool checked);
    void onAutoSyncChanged(bool autoSync);
    void onHideSyncWidgets();

public Q_SLOTS:
    void SyncTimeLbl(bool checked);

private:
    QVBoxLayout *m_mainLayout;
    dcc::widgets::SwitchWidget *m_autoSyncSwitch;
    DTK_WIDGET_NAMESPACE::DListView *m_listView;
    dcc::cloudsync::SyncStateIcon *m_stateIcon;
    QLabel *m_stateLbl;
    QLabel *m_lastSyncTimeLbl;
    qlonglong m_lastSyncTime;
    QStandardItemModel *m_listModel;
    QMap<dcc::cloudsync::SyncType, QStandardItem *> m_itemMap;
    DTK_WIDGET_NAMESPACE::DTipLabel *m_networkTip;
    std::pair<qint32, QString> m_state;
    QLabel *m_lab;
};
} // namespace sync
} // namespace DCC_NAMESPACE
