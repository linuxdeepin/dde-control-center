// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include <DFloatingButton>
#include <DListView>
#include <DStandardItem>

#include <QWidget>
#include <QGSettings>

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

    void setShowDefaultAccountInfo(bool showDefaultAccountInfo);
public Q_SLOTS:
    void addUser(dcc::accounts::User *user, bool t1 = true);
    void removeUser(dcc::accounts::User *user);
    void onItemClicked(const QModelIndex &index);
    void onFullNameEnableChanged(const QString &key);
    void onShowSafetyPage(const QString &errorTips);

Q_SIGNALS:
    void requestShowAccountsDetail(dcc::accounts::User *account);
    void requestCreateAccount();
    void requestShowLastClickedUserInfo(bool t = false);
    void requestBack();
    void requestLoadUserList();
    void requestUpdatGroupList();

private:
    DTK_WIDGET_NAMESPACE::DFloatingButton *m_createBtn;
    dcc::widgets::MultiSelectListView *m_userlistView;
    QStandardItemModel *m_userItemModel;
    dcc::accounts::UserModel *m_userModel;
    QList<dcc::accounts::User *> m_userList;
    bool m_isShowFirstUserInfo = false;
    int m_saveClickedRow;
    QGSettings *m_accountSetting{nullptr};
    bool m_isCreateValid;
    bool m_showDefaultAccountInfo;
};

}   // namespace accounts
}   // namespace dccV20
