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
    , m_proxyModel(new MySortFilterProxyModel)
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
//    m_userlistView->setModel(m_userItemModel);

    m_proxyModel->setSourceModel(m_userItemModel);
    m_proxyModel->setSortRole(AccountsWidget::ItemDataRole);
    m_proxyModel->sort(0, Qt::AscendingOrder);
    m_userlistView->setModel(m_proxyModel);

    setLayout(mainContentLayout);

    connect(m_userlistView, &QListView::clicked, this, &AccountsWidget::onItemClicked);
    connect(m_createBtn, &QPushButton::clicked, this, &AccountsWidget::requestCreateAccount);
}

void AccountsWidget::setModel(UserModel *model)
{
    connect(model, &UserModel::userAdded, this, &AccountsWidget::addUser);
    connect(model, &UserModel::userRemoved, this, &AccountsWidget::removeUser);
    //给账户列表添加用户数据
    for (auto user : model->userList()) {
        addUser(user);
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

void AccountsWidget::addUser(User *user)
{
    DStandardItem *item = new DStandardItem;

    if (user->currentAvatar().startsWith("file://")) {
        item->setIcon(QIcon(QUrl(user->currentAvatar()).toLocalFile()));
        m_isShowFirstUserInfo = true;
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

    item->setEditable(false);
    m_userItemModel->appendRow(item);
    m_userList << user;

    if (user->isCurrentUser()) {
        doTopCurrentUser(user);
    }

    connectUserWithItem(user);

    connect(user, &User::isCurrentUserChanged, this, [ = ](bool isCurrentUser) {
        if (isCurrentUser) {
            doTopCurrentUser(user);
            showDefaultAccountInfo();
        }
    });

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

void AccountsWidget::doTopCurrentUser(User *user)
{
    //判断是不是位于第一个
    int currentRow = m_userList.indexOf(user);
    if (currentRow == 0) {
        return;
    }

    //用户列表中必须更新当前与item关联user的位置
    User *user_clickrow = m_userList.at(currentRow);
    m_userList.removeOne(user_clickrow);
    m_userList.push_front(user_clickrow);

    //获取当前选中item上面的所有数据
    QString name_clickrow = m_userItemModel->index(currentRow, 0).data(Qt::DisplayRole).toString();
    QIcon pic_clickrow = m_userItemModel->index(currentRow, 0).data(Qt::DecorationRole).value<QIcon>();
    quint64 data_clickrow = m_userItemModel->index(currentRow, 0).data(AccountsWidget::ItemDataRole).toULongLong();

    //创建新的item，设置数据。
    QStandardItem *newItem = new QStandardItem;
    newItem->setText(name_clickrow);
    newItem->setIcon(pic_clickrow);
    newItem->setData(QVariant::fromValue(data_clickrow), AccountsWidget::ItemDataRole);

    //在首行新增item，删除旧item
    m_userItemModel->removeRow(currentRow);
    m_userItemModel->insertRow(0, newItem);

    //重新给第一个item关联user信号
    user_clickrow->disconnect(SIGNAL(nameChanged(const QString &)));
    user_clickrow->disconnect(SIGNAL(fullnameChanged(const QString &)));
    user_clickrow->disconnect(SIGNAL(currentAvatarChanged(const QString &)));
    user_clickrow->disconnect(SIGNAL(createdTimeChanged(const quint64 &)));
    connectUserWithItem(user_clickrow);
}

void AccountsWidget::connectUserWithItem(User *user)
{
    int index = m_userList.indexOf(user);
    auto item = m_userItemModel->item(index);
    connect(user, &User::nameChanged, this, [ = ](const QString &) {
        item->setText(user->displayName());
    });
    connect(user, &User::fullnameChanged, this, [ = ](const QString &) {
        //对用户全名做限制，如果长度超过32，就在后面显示...
        QString fullname = user->displayName();
        if (fullname.length() > 32) {
            QString newfullname = fullname.left(32) + QString("...");
            item->setText(newfullname);
        } else {
            item->setText(fullname);
        }
    });
    connect(user, &User::currentAvatarChanged, this, [ = ](const QString & avatar) {
        if (avatar.startsWith("file://")) {
            item->setIcon(QIcon(QUrl(avatar).toLocalFile()));
        } else {
            item->setIcon(QIcon(avatar));
        }
    });
    connect(user, &User::createdTimeChanged, this, [ = ](const quint64 & createdtime){
        item->setData(QVariant::fromValue(createdtime), AccountsWidget::ItemDataRole);
    });
}
