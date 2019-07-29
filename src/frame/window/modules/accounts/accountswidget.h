/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

class QPushButton;
class QVBoxLayout;
class QListView;
class QStandardItem;
class QStandardItemModel;

namespace dcc {
namespace accounts {
class User;
class UserModel;
}
}

namespace DCC_NAMESPACE {
namespace accounts {
class AccountsDetailWidget;
//显示用户列表
class AccountsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountsWidget(QWidget *parent = nullptr);
    void setModel(dcc::accounts::UserModel *model);

public Q_SLOTS:
    void addUser(dcc::accounts::User *user);
    void removeUser(dcc::accounts::User *user);
    void onItemClicked(const QModelIndex &index);

Q_SIGNALS:
    void requestShowAccountsDetail(dcc::accounts::User *account);
    void requestCreateAccount();

private:
    QPushButton *m_createBtn;
    QListView *m_userlistView;
    QStandardItemModel *m_userItemModel;
    QList<dcc::accounts::User*> m_userList;
};

}   // namespace accounts
}   // namespace dccV20
