// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "modules/accounts/user.h"
#include "modules/accounts/usermodel.h"
#include "accountswidget.h"
#include "widgets/contentwidget.h"

#include <QLabel>

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {
namespace accounts {

class UserGroupsPage : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit UserGroupsPage(dcc::accounts::User *user, dcc::accounts::UserModel *userModel, dcc::ContentWidget *parent = nullptr);
    ~UserGroupsPage();

Q_SIGNALS:
    void requestSetGroups(dcc::accounts::User *user, const QStringList &usrGroups);

private Q_SLOTS:
    void changeUserGroup(const QStringList &groups);
    void userGroupClicked(const QModelIndex &index);
    void setGroupInfo(const QStringList &group);
    void onGidChanged(const QString &gid);

private:
    void initWidget();
    void initData();

private:
    QString m_groupName;
    QLabel *m_groupTip;
    QVBoxLayout * m_layout;
    dcc::accounts::User *m_curUser;
    dcc::accounts::UserModel *m_userModel;
    DTK_WIDGET_NAMESPACE::DListView *m_groupListView;
    QStandardItemModel *m_groupItemModel;
};

}
}
