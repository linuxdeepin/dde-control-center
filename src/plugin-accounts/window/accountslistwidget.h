/*
 * Copyright (C) 2021 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     Tianlu Shao <shaotianlu@uniontech.com>
 *
 * Maintainer: Tianlu Shao <shaotianlu@uniontech.com>
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
#include "src/plugin-accounts/operation/user.h"
#include "src/plugin-accounts/operation/usermodel.h"

#include <DFloatingButton>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QStandardItem;
class QStandardItemModel;
class QListWidget;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
class User;
class UserModel;
class AccountsListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AccountsListWidget(QWidget *parent = nullptr);
    ~AccountsListWidget();
    void setModel(UserModel *model);
    void showDefaultAccountInfo();
    void showLastAccountInfo();
    void setShowFirstUserInfo(bool show);
    void connectUserWithItem(User *user);
    void setShowDefaultAccountInfo(bool showDefaultAccountInfo);
    void handleRequestBack(UserModel::ActionOption option = UserModel::ClickCancel);
public Q_SLOTS:
    void addUser(User *user, bool t1 = true);
    void removeUser(User *user);
    void onItemClicked(const QModelIndex &index);

Q_SIGNALS:
    void requestShowAccountsDetail(User *account);
    void requestCreateAccount();
    void requestShowLastClickedUserInfo(bool t = false);
    void requestBack();
    void requestLoadUserList();
    void requestUpdatGroupList();

private:
    DTK_WIDGET_NAMESPACE::DFloatingButton *m_createBtn;
    QListWidget *m_userlistView;
    UserModel *m_userModel;
    QList<User *> m_userList;
    bool m_isShowFirstUserInfo = false;
    int m_saveClickedRow;
    bool m_showDefaultAccountInfo;
};

}   // namespace DCC_NAMESPACE
