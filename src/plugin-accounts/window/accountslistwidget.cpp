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
#include "accountslistwidget.h"
#include "src/plugin-accounts/operation/usermodel.h"
#include "src/plugin-accounts/operation//user.h"
#include "accountsdetailwidget.h"
#include "onlineicon.h"
#include "useritemdelegate.h"

#include <DStyleOption>
#include <DFloatingButton>
#include <DSysInfo>

#include <QWidget>
#include <QVBoxLayout>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTimer>
#include <QDebug>
#include <QIcon>
#include <QSize>
#include <QPainter>
#include <QPixmap>
#include <QBitmap>
#include <QPainterPath>
#include <QScroller>
#include <QScrollBar>
#include <QPushButton>

#include <DApplicationHelper>
#include <DListWidget>

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE
AccountsListWidget::AccountsListWidget(QWidget *parent)
    : QWidget(parent)
    , m_createBtn(new DFloatingButton(DStyle::SP_IncreaseElement, this))
    , m_userlistView(new QListWidget(this))
    , m_saveClickedRow(0)
    , m_showDefaultAccountInfo(true)
{
    m_createBtn->setFixedSize(50, 50);
    m_createBtn->setToolTip(tr("Create Account"));
    m_createBtn->setAccessibleName(tr("Create Account"));
    m_createBtn->setVisible(true);

    QHBoxLayout *mainContentLayout = new QHBoxLayout();
    mainContentLayout->setMargin(0);
    mainContentLayout->addWidget(m_userlistView, Qt::AlignLeft);
    mainContentLayout->addWidget(m_createBtn, 0, Qt::AlignRight);
    setMaximumHeight(90);

    m_userlistView->setSpacing(15);
    m_userlistView->setMaximumHeight(90);
    m_userlistView->setFrameShape(QFrame::NoFrame);

    QPalette pa = m_userlistView->palette();
    pa.setColor(QPalette::Base, pa.color(QPalette::Window));
    m_userlistView->setAutoFillBackground(true);
    m_userlistView->setPalette(pa);

    m_userlistView->setViewMode(QListView::ListMode);
    m_userlistView->setFlow(QListView::LeftToRight);
    m_userlistView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_userlistView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_userlistView->setHorizontalScrollMode(QListWidget::ScrollPerPixel);
    QScroller::grabGesture(m_userlistView,QScroller::LeftMouseButtonGesture);
    UserItemDelegate *delegate = new UserItemDelegate(m_userlistView);
    m_userlistView->setItemDelegate(delegate);

    QScroller::grabGesture(m_userlistView->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(m_userlistView);
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    setLayout(mainContentLayout);

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [=](DGuiApplicationHelper::ColorType themeType){
        //在切换主题的时候，combox无法获取当前是hidepopup还是showpopup, 所以需要设置为hidepopup来达到更新图标的目的
        Q_UNUSED(themeType)
        QPalette pa = m_userlistView->palette();
        pa.setColor(QPalette::Base, pa.color(QPalette::Window));
        m_userlistView->setAutoFillBackground(true);
        m_userlistView->setPalette(pa);
    });
    //自动刷新当前链接状态
    connect(m_userlistView, &QListWidget::currentItemChanged, this, [&](QListWidgetItem *current, QListWidgetItem *previous) {
        current->setData(UserItemDelegate::SelectedRole, true);
        onItemClicked(m_userlistView->currentIndex());
        if(previous)
            previous->setData(UserItemDelegate::SelectedRole, false);
    });
    connect(m_userlistView, &QListView::clicked, this, &AccountsListWidget::onItemClicked);
    connect(m_userlistView, &DListView::activated, m_userlistView, &QListView::clicked);
    connect(m_userlistView->verticalScrollBar(), &QScrollBar::valueChanged, this, [ = ](int value) {
        static int valueTemp = 0;
        if (value > valueTemp) {
            requestLoadUserList();
        }
        valueTemp = value;
    });
    connect(m_createBtn, &QPushButton::clicked, this, &AccountsListWidget::requestCreateAccount);
}

AccountsListWidget::~AccountsListWidget()
{
}

void AccountsListWidget::setModel(UserModel *model)
{
    m_userModel = model;
    if (m_userModel->getIsSecurityHighLever()) {
        for (auto user : m_userModel->userList()) {
            if (user->name() == m_userModel->getCurrentUserName() && !(user->securityLever() == SecurityLever::Sysadm)) {
                m_createBtn->setVisible(false);
                break;
            }
        }
    }

    connect(model, &UserModel::userAdded, this, [this](User * user) {
        addUser(user);
    });
    connect(model, &UserModel::userRemoved, this, &AccountsListWidget::removeUser);
    //给账户列表添加用户数据
    for (auto user : model->userList()) {
        addUser(user, false);
    }
}

void AccountsListWidget::showDefaultAccountInfo()
{
    qDebug() << "m_showDefaultAccountInfo" << m_showDefaultAccountInfo;
    if (m_userlistView->count() > 0 && m_showDefaultAccountInfo) {
        QModelIndex qindex = m_userlistView->model()->index(0, 0);
        m_userlistView->setCurrentIndex(qindex);
        Q_EMIT m_userlistView->clicked(qindex);
    }
}

void AccountsListWidget::showLastAccountInfo()
{
    if (m_userlistView->count() > 0) {
        int lastindex = m_userlistView->model()->rowCount() - 1;
        QModelIndex qindex = m_userlistView->model()->index(lastindex, 0);
        m_userlistView->setFocus();
        m_userlistView->setCurrentIndex(qindex);
        Q_EMIT m_userlistView->clicked(qindex);
    }
}

void AccountsListWidget::setShowFirstUserInfo(bool show)
{
    m_isShowFirstUserInfo = show;
}

void AccountsListWidget::addUser(User *user, bool t1)
{
    //active
    m_userList << user;
    QListWidgetItem *item = new QListWidgetItem();
    item->setData(UserItemDelegate::NameRole, user->displayName().isEmpty()? user->name(): user->displayName());
    auto path = user->currentAvatar();
    path = path.startsWith("file://") ? QUrl(path).toLocalFile() : path;
    item->setData(UserItemDelegate::IconRole, path);
    item->setData(UserItemDelegate::OnlineStatusRole, user->online());
    item->setData(UserItemDelegate::SelectedRole, false);
    if(user->isCurrentUser()) {
        m_userlistView->insertItem(0, item);
        m_userList.push_front(user);
        m_userList.pop_back();
    } else {
        m_userlistView->addItem(item);
    }
    item->setSizeHint(QSize(55, 75));
    m_userlistView->openPersistentEditor(item);
    connect(user, &User::onlineChanged, this, [ = ](const bool &online) {
        item->setData(UserItemDelegate::OnlineStatusRole, online);
    });
    connectUserWithItem(user);

    connect(user, &User::isCurrentUserChanged, this, [ = ](bool isCurrentUser) {
        if (isCurrentUser) {

            auto tindex = m_userList.indexOf(user);
            auto titem = m_userlistView->takeItem(tindex);

            m_userlistView->insertItem(0, titem);

            m_userList.removeOne(user);
            m_userList.push_front(user);
            showDefaultAccountInfo();
        }
    });

    if (t1)
        return;
}

void AccountsListWidget::removeUser(User *user)
{
    QListWidgetItem *item = m_userlistView->takeItem(m_userList.indexOf(user));
    delete item;

    m_userList.removeOne(user);

    if (m_userList.isEmpty()) {
        Q_EMIT requestBack();
        return;
    }

    m_isShowFirstUserInfo ? showDefaultAccountInfo() : showLastAccountInfo();
}

void AccountsListWidget::onItemClicked(const QModelIndex &index)
{
    if (DSysInfo::UosServer == DSysInfo::uosType()) {
        Q_EMIT requestUpdatGroupList();
    }

    m_saveClickedRow = index.row();
    Q_EMIT requestShowAccountsDetail(m_userList[index.row()]);
}

void AccountsListWidget::connectUserWithItem(User *user)
{
    connect(user, &User::nameChanged, this, [ = ](const QString &) {
        int tindex = m_userList.indexOf(user);
        auto titem = m_userlistView->item(tindex);
        if (!titem) {
            return;
        }
        titem->setText(user->displayName());
    });
    connect(user, &User::fullnameChanged, this, [ = ](const QString &) {
        int tindex = m_userList.indexOf(user);
        auto titem = m_userlistView->item(tindex);
        qInfo() << "fullnameChanged" << user->fullname() << tindex;
        if (titem) {
            qInfo() << "setData->UserItemDelegate::NameRole";
            titem->setData(UserItemDelegate::NameRole, user->displayName().isEmpty()? user->name(): user->displayName());
        }
    });
    connect(user, &User::currentAvatarChanged, this, [ = ](const QString & avatar) {
        int tindex = m_userList.indexOf(user);
        auto titem = m_userlistView->item(tindex);
        if (!titem) {
            return;
        }
        auto ratio = this->devicePixelRatioF();
        auto path = avatar;
        if (ratio > 4.0) {
            path.replace("icons/", "icons/bigger/");
        }

        path = path.startsWith("file://") ? QUrl(path).toLocalFile() : path;
        titem->setData(UserItemDelegate::IconRole, path);
    });
}

void AccountsListWidget::handleRequestBack(UserModel::ActionOption option)
{
    switch (option) {
    case UserModel::ClickCancel: { //点击取消
        QModelIndex qindex0 = m_userlistView->model()->index(m_saveClickedRow, 0);
        m_userlistView->setFocus();
        m_userlistView->setCurrentIndex(qindex0);
        onItemClicked(qindex0);
        emit requestBack();
    }
    break;
    case UserModel::CreateUserSuccess: { //创建账户成功
        QModelIndex qindex1 = m_userlistView->model()->index(m_userlistView->model()->rowCount() - 1, 0);
        m_userlistView->setFocus();
        m_userlistView->setCurrentIndex(qindex1);
        onItemClicked(qindex1);
        emit requestBack();
    }
    break;
    case UserModel::ModifyPwdSuccess: { //修改密码成功
        QModelIndex qindex2 = m_userlistView->model()->index(0, 0);
        m_userlistView->setFocus();
        m_userlistView->setCurrentIndex(qindex2);
        onItemClicked(qindex2);
        emit requestBack();
    }
    break;
    }
}

void AccountsListWidget::setShowDefaultAccountInfo(bool showDefaultAccountInfo)
{
    m_showDefaultAccountInfo = showDefaultAccountInfo;
}

