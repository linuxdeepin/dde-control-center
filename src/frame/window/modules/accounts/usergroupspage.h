/*
 * Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     dengbo <dengbo@uniontech.com>
 *
 * Maintainer: dengbo <dengbo@uniontech.com>
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

#include "modules/accounts/user.h"
#include "modules/accounts/usermodel.h"
#include "accountswidget.h"

#include <QLabel>

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {
namespace accounts {

class UserGroupsPage : public QWidget
{
    Q_OBJECT
public:
    explicit UserGroupsPage(dcc::accounts::User *user, dcc::accounts::UserModel *userModel, QWidget *parent = nullptr);
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
