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

#include "accountswidget.h"

#include "modules/accounts/usermodel.h"
#include "modules/accounts/user.h"
#include "accountsdetailwidget.h"

#include <DStyleOption>
#include <DStandardItem>

#include <QWidget>
#include <QVBoxLayout>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTimer>
#include <QDebug>
#include <QIcon>
#include <QSize>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace DCC_NAMESPACE::accounts;

AccountsWidget::AccountsWidget(QWidget *parent)
    : QWidget(parent)
    , m_createBtn(new DFloatingButton(DStyle::SP_IncreaseElement, this))
    , m_userlistView(new DListView)
    , m_userItemModel(new QStandardItemModel)
{
    setObjectName("Accounts");

    m_createBtn->setFixedSize(50, 50);

    QVBoxLayout *mainContentLayout = new QVBoxLayout();
    mainContentLayout->addWidget(m_userlistView);
    mainContentLayout->addWidget(m_createBtn, 0, Qt::AlignHCenter);

    m_userlistView->setFrameShape(QFrame::NoFrame);
    m_userlistView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_userlistView->setEditTriggers(QListView::NoEditTriggers);
    m_userlistView->setDragEnabled(false);
    m_userlistView->setIconSize(QSize(30, 30));
    m_userlistView->setLayoutDirection(Qt::LeftToRight);
    m_userlistView->setModel(m_userItemModel);

    setLayout(mainContentLayout);

    connect(m_userlistView, &QListView::clicked, this, &AccountsWidget::onItemClicked);
    connect(m_createBtn, &QPushButton::clicked, this, &AccountsWidget::requestCreateAccount);
}

void AccountsWidget::setModel(UserModel *model)
{
    m_model = model;

    m_userList << nullptr;
    m_userItemModel->appendRow(new DStandardItem);

    connect(model, &UserModel::userAdded, this, [this](User * user) {
        addUser(user);
    });
    connect(model, &UserModel::userRemoved, this, &AccountsWidget::removeUser);
    //给账户列表添加用户数据
    for (auto user : model->userList()) {
        addUser(user, false);
    }
}

void AccountsWidget::showDefaultAccountInfo()
{
    QModelIndex qindex = m_userItemModel->index(0, 0);
    m_userlistView->setFocus();
    m_userlistView->setCurrentIndex(qindex);
    Q_EMIT m_userlistView->clicked(qindex);
}

void AccountsWidget::showLastAccountInfo()
{
    int lastindex = m_userItemModel->rowCount() - 1;
    QModelIndex qindex = m_userItemModel->index(lastindex, 0);
    m_userlistView->setFocus();
    m_userlistView->setCurrentIndex(qindex);
    Q_EMIT m_userlistView->clicked(qindex);
}

void AccountsWidget::setShowFirstUserInfo(bool show)
{
    m_isShowFirstUserInfo = show;
}

void AccountsWidget::addUser(User *user, bool t1)
{
    //active
    m_userList << user;
    DStandardItem *item = new DStandardItem;
    item->setData(0, AccountsWidget::ItemDataRole);
    m_userItemModel->appendRow(item);

    connectUserWithItem(user);

    connect(user, &User::isCurrentUserChanged, this, [ = ](bool isCurrentUser) {
        if (isCurrentUser) {
            auto idx = m_userList.indexOf(user);
            auto tttitem = m_userItemModel->takeRow(idx);
            Q_ASSERT(tttitem[0] == item);

            m_userItemModel->removeRow(0);
            m_userItemModel->insertRow(0, item);

            m_userList.removeOne(user);
            m_userList[0] = user;
        }
    });

    if (t1) {
        return;
    }

    if (user->currentAvatar().startsWith("file://")) {
        item->setIcon(QIcon(QUrl(user->currentAvatar()).toLocalFile()));
    } else {
        item->setIcon(QIcon(user->currentAvatar()));
    }

    //对用户全名做限制，如果长度超过32，就在后面显示...
    QString fullname = user->displayName();
    if (fullname.length() > 32) {
        QString newfullname = fullname.left(32) + QString("...");
        item->setText(newfullname);
    } else {
        item->setText(fullname);
    }

    if (user->isCurrentUser()) {
        //如果是当前用户
        auto tttitem = m_userItemModel->takeRow(m_userItemModel->rowCount() - 1);
        Q_ASSERT(tttitem[0] == item);
        m_userItemModel->removeRow(0);
        m_userItemModel->insertRow(0, item);

        m_userList[0] = user;
        m_userList.pop_back();
    } else {
        int count = m_userItemModel->rowCount();
        for (int idx = 1; idx < count; ++idx) {
            if (user->createdTime() < m_userList[idx]->createdTime()) {
                auto tttitem = m_userItemModel->takeRow(count - 1);
                Q_ASSERT(tttitem[0] == item);
                m_userItemModel->insertRow(idx, item);

                m_userList.insert(idx, user);
                m_userList.pop_back();
                break;
            }
        }
    }

    //The first line in the default selection when the user list is loaded
    if (m_isShowFirstUserInfo) {
        showDefaultAccountInfo();
    } else {
        showLastAccountInfo();
    }
}

void AccountsWidget::removeUser(User *user)
{
    m_userItemModel->removeRow(m_userList.indexOf(user)); // It will delete when remove
    m_userList.removeOne(user);

    if (m_isShowFirstUserInfo) {
        showDefaultAccountInfo();
    } else {
        showLastAccountInfo();
    }
}

void AccountsWidget::onItemClicked(const QModelIndex &index)
{
    Q_EMIT requestShowAccountsDetail(m_userList[index.row()]);
}

void AccountsWidget::connectUserWithItem(User *user)
{
    connect(user, &User::nameChanged, this, [ = ](const QString &) {
        int tindex = m_userList.indexOf(user);
        auto titem = m_userItemModel->item(tindex);
        if (!titem) {
            return;
        }
        titem->setText(user->displayName());
    });
    connect(user, &User::fullnameChanged, this, [ = ](const QString &) {
        int tindex = m_userList.indexOf(user);
        auto titem = m_userItemModel->item(tindex);
        if (!titem) {
            return;
        }
        //对用户全名做限制，如果长度超过32，就在后面显示...
        QString fullname = user->displayName();
        if (fullname.length() > 32) {
            QString newfullname = fullname.left(32) + QString("...");
            titem->setText(newfullname);
        } else {
            titem->setText(fullname);
        }
    });
    connect(user, &User::currentAvatarChanged, this, [ = ](const QString & avatar) {
        int tindex = m_userList.indexOf(user);
        auto titem = m_userItemModel->item(tindex);
        if (!titem) {
            return;
        }
        if (avatar.startsWith("file://")) {
            titem->setIcon(QIcon(QUrl(avatar).toLocalFile()));
        } else {
            titem->setIcon(QIcon(avatar));
        }
    });

    connect(user, &User::createdTimeChanged, this, [ = ](const quint64 & createdtime) {
        int tindex = m_userList.indexOf(user);
        auto titem = m_userItemModel->item(tindex);
        if (!titem) {
            return;
        }
        titem->setData(QVariant::fromValue(createdtime), AccountsWidget::ItemDataRole);

        for (int i = 1; i < m_userItemModel->rowCount(); i++) {
            quint64 icreatedtime = m_userItemModel->index(i, 0).data(AccountsWidget::ItemDataRole).toULongLong();
            if (createdtime < icreatedtime) {
                m_userItemModel->insertRow(i, titem);
                m_userItemModel->removeRow(tindex + 1);

                m_userList.insert(i, user);
                m_userList.removeAt(tindex + 1);
                break;
            }
        }
    });
}
