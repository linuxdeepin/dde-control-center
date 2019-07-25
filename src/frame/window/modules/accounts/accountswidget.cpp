/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
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

#include "accountswidget.h"
#include "modules/accounts/usermodel.h"
#include "modules/accounts/user.h"

#include <QPushButton>
#include <QDebug>
#include <QIcon>

using namespace DCC_NAMESPACE::accounts;

AccountsWidget::AccountsWidget(QWidget *parent)
    : QWidget(parent)
    , m_createBtn(new QPushButton(tr("Create Account")))
    , m_userlistView(new QListView)
    , m_userItemModel(new QStandardItemModel(this))
{
    setObjectName("Accounts");

    QVBoxLayout *mainContentLayout = new QVBoxLayout(this);
    mainContentLayout->addWidget(m_userlistView);
    mainContentLayout->addWidget(m_createBtn);

    m_userlistView->setModel(m_userItemModel);
}

void AccountsWidget::setModel(UserModel *model)
{
    //给账户列表添加用户数据
    for (auto user : model->userList()) {
        addUser(user);
    }
}

void AccountsWidget::addUser(User *user)
{
    QStandardItem *pItem = new QStandardItem;
    pItem->setIcon(QIcon(user->currentAvatar()));
    pItem->setText(user->name());
    m_userItemModel->appendRow(pItem);
}
