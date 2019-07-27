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

#include "window/namespace.h"

#include <QWidget>

class QListView;
class QVBoxLayout;
class QStandardItemModel;
class QStandardItem;
class QLabel;

namespace dcc {
namespace widgets {
class SwitchWidget;
}
namespace cloudsync {
class SyncModel;
}
}

namespace DCC_NAMESPACE {
namespace sync {
class IndexPage : public QWidget {
    Q_OBJECT
public:
    IndexPage(QWidget *parent = nullptr);
    void setModel(dcc::cloudsync::SyncModel *model);

Q_SIGNALS:
    void requestSetAutoSync(bool enable) const;

private:
    void onListViewClicked(const QModelIndex &index);
    void onUserInfoChanged(const QVariantMap& infos);

private:
    QVBoxLayout *m_mainLayout;
    dcc::cloudsync::SyncModel *m_model;
    QLabel* m_avatar;
    QLabel* m_username;
    dcc::widgets::SwitchWidget* m_autoSyncSwitch;
    QListView *m_listView;
    QStandardItemModel *m_listModel;
    QList<QStandardItem*> m_items;
};
} // namespace sync
} // namespace DCC_NAMESPACE
