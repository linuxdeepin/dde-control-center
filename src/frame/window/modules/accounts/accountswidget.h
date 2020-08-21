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

#include "interface/namespace.h"

#include <DFloatingButton>
#include <DListView>
#include <DStandardItem>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QStandardItem;
class QStandardItemModel;
QT_END_NAMESPACE

namespace dcc {
namespace accounts {
class User;
class UserModel;
}
namespace widgets {
class MultiSelectListView;
}
}

namespace DCC_NAMESPACE {
namespace accounts {
class AccountsDetailWidget;
class MySortFilterProxyModel;
//显示用户列表
class AccountsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountsWidget(QWidget *parent = nullptr);
    virtual ~AccountsWidget();
    void setModel(dcc::accounts::UserModel *model);
    void showDefaultAccountInfo();
    void showLastAccountInfo();
    void setShowFirstUserInfo(bool show);
    void connectUserWithItem(dcc::accounts::User *user);
    void selectUserList();

    enum AccountRole {
        ItemDataRole = Dtk::UserRole + 1
    };

    enum ActionOption {
        ClickCancel = 0,
        CreateUserSuccess,
        ModifyPwdSuccess
    };

    QPixmap pixmapToRound(const QPixmap &src);
    void handleRequestBack(AccountsWidget::ActionOption option = AccountsWidget::ClickCancel);

public Q_SLOTS:
    void addUser(dcc::accounts::User *user, bool t1 = true);
    void removeUser(dcc::accounts::User *user);
    void onItemClicked(const QModelIndex &index);

Q_SIGNALS:
    void requestShowAccountsDetail(dcc::accounts::User *account);
    void requestCreateAccount();
    void requestShowLastClickedUserInfo(bool t = false);

private:
    DTK_WIDGET_NAMESPACE::DFloatingButton *m_createBtn;
    dcc::widgets::MultiSelectListView *m_userlistView;
    QStandardItemModel *m_userItemModel;
    dcc::accounts::UserModel *m_userModel;
    QList<dcc::accounts::User *> m_userList;
    bool m_isShowFirstUserInfo = false;
    bool m_currentUserAdded = false;
    int m_saveClickedRow;
};

}   // namespace accounts
}   // namespace dccV20
