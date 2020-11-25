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
#include "widgets/multiselectlistview.h"
#include "modules/accounts/usermodel.h"
#include "modules/accounts/user.h"
#include "accountsdetailwidget.h"
#include "window/utils.h"
#include "onlineicon.h"

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
#include <QPainter>
#include <QPixmap>
#include <QBitmap>
#include <QPainterPath>
#include <QScroller>
#include <QScrollBar>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace DCC_NAMESPACE::accounts;
#define GSETTINGS_SHOW_CREATEUSER "show-createuser"

AccountsWidget::AccountsWidget(QWidget *parent)
    : QWidget(parent)
    , m_createBtn(new DFloatingButton(DStyle::SP_IncreaseElement, this))
    , m_userlistView(new dcc::widgets::MultiSelectListView(this))
    , m_userItemModel(new QStandardItemModel(this))
    , m_saveClickedRow(0)
{
    m_createBtn->setFixedSize(50, 50);
    //~ contents_path /accounts/New Account
    m_createBtn->setToolTip(tr("Create Account"));
    m_createBtn->setAccessibleName(tr("Create Account"));

    m_accountSetting = new QGSettings("com.deepin.dde.control-center", QByteArray(), this);
    m_isCreateValid = m_accountSetting->get(GSETTINGS_SHOW_CREATEUSER).toBool();

    m_createBtn->setVisible(m_isCreateValid);

    QVBoxLayout *mainContentLayout = new QVBoxLayout();
    mainContentLayout->setMargin(0);
    mainContentLayout->addWidget(m_userlistView);
    mainContentLayout->addWidget(m_createBtn, 0, Qt::AlignHCenter);

    m_userlistView->setAccessibleName("List_userlist");
    m_userlistView->setFrameShape(QFrame::NoFrame);
    m_userlistView->setViewportMargins(ScrollAreaMargins);
    m_userlistView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_userlistView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_userlistView->setEditTriggers(QListView::NoEditTriggers);
    m_userlistView->setDragEnabled(false);
    m_userlistView->setIconSize(QSize(30, 30));
    m_userlistView->setLayoutDirection(Qt::LeftToRight);
    m_userlistView->setModel(m_userItemModel);

    QScroller *scroller = QScroller::scroller(m_userlistView->viewport());
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    setLayout(mainContentLayout);

    connect(m_userlistView, &QListView::clicked, this, &AccountsWidget::onItemClicked);
    connect(m_userlistView, &DListView::activated, m_userlistView, &QListView::clicked);
    connect(m_userlistView->verticalScrollBar(), &QScrollBar::valueChanged, this, [ = ](int value) {
        static int valueTemp = 0;
        if (value > valueTemp) {
            requestLoadUserList();
        }
        valueTemp = value;
    });
    connect(m_createBtn, &QPushButton::clicked, this, &AccountsWidget::requestCreateAccount);
}

AccountsWidget::~AccountsWidget()
{
    if (m_userItemModel) {
        m_userItemModel->clear();
        m_userItemModel->deleteLater();
        m_userItemModel = nullptr;
    }
}

void AccountsWidget::setModel(UserModel *model)
{
    m_userModel = model;

    m_createBtn->setVisible(m_userModel->isCreateUserValid() && m_isCreateValid);

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
    if (m_userlistView->count() > 0) {
        QModelIndex qindex = m_userItemModel->index(0, 0);
        m_userlistView->setFocus();
        m_userlistView->setCurrentIndex(qindex);
        Q_EMIT m_userlistView->clicked(qindex);
    }
}

void AccountsWidget::showLastAccountInfo()
{
    if (m_userlistView->count() > 0) {
        int lastindex = m_userItemModel->rowCount() - 1;
        QModelIndex qindex = m_userItemModel->index(lastindex, 0);
        m_userlistView->setFocus();
        m_userlistView->setCurrentIndex(qindex);
        Q_EMIT m_userlistView->clicked(qindex);
    }
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

    /* 用户列表显示用户类型 */
    auto *subTitleAction = new DViewItemAction;
    if (user->userType() == User::UserType::Administrator) {
        subTitleAction->setText(tr("Administrator"));
    } else {
        subTitleAction->setText(tr("Standard User"));
    }
    subTitleAction->setFontSize(DFontSizeManager::T8);
    subTitleAction->setTextColorRole(DPalette::TextTips);
    item->setTextActionList({subTitleAction});

    connect(user, &User::userTypeChanged, this, [ = ](int userType) {
        if (userType == User::UserType::Administrator) {
            subTitleAction->setText(tr("Administrator"));
        } else {
            subTitleAction->setText(tr("Standard User"));
        }
    });

    DViewItemAction *onlineFlag = new DViewItemAction(Qt::AlignCenter | Qt::AlignRight, QSize(), QSize(), true);
    OnlineIcon *onlineIcon = new OnlineIcon(m_userlistView->viewport());
    onlineIcon->setFixedSize(12, 12);
    onlineFlag->setWidget(onlineIcon);
    item->setActionList(Qt::Edge::RightEdge, {onlineFlag});
    onlineFlag->setVisible(user->online());
    if (onlineFlag->widget()) {
        onlineFlag->widget()->setVisible(onlineFlag->isVisible());
    }
    connect(user, &User::onlineChanged, this, [=](const bool &online) {
        onlineFlag->setVisible(online);
        if (onlineFlag->widget()) {
            onlineFlag->widget()->setVisible(onlineFlag->isVisible());
        }
    });

    m_userItemModel->appendRow(item);
    connectUserWithItem(user);

    connect(user, &User::isCurrentUserChanged, this, [ = ](bool isCurrentUser) {
        if (isCurrentUser) {
            auto tindex = m_userList.indexOf(user);
            auto titem = m_userItemModel->takeRow(tindex);

            m_userItemModel->insertRow(0, titem);

            m_userList.removeOne(user);
            m_userList.push_front(user);

            showDefaultAccountInfo();
        }
    });

    if (t1)
        return;

    auto path = user->currentAvatar();
    path = path.startsWith("file://") ? QUrl(path).toLocalFile() : path;
    QPixmap pixmap = pixmapToRound(path);

    item->setIcon(QIcon(pixmap));
    item->setText(user->displayName());

    if (user->isCurrentUser()) {
        //如果是当前用户
        auto tttitem = m_userItemModel->takeRow(m_userItemModel->rowCount() - 1);
        Q_ASSERT(tttitem[0] == item);
        m_userItemModel->insertRow(0, item);

        m_userList.push_front(user);
        m_userList.pop_back();
        m_currentUserAdded = true;

        QTimer::singleShot(0, this, &AccountsWidget::showDefaultAccountInfo);
    }
}

void AccountsWidget::removeUser(User *user)
{
    m_userItemModel->removeRow(m_userList.indexOf(user)); // It will delete when remove
    m_userList.removeOne(user);

    if (m_userList.isEmpty()) {
        Q_EMIT requestBack();
        return;
    }

    m_isShowFirstUserInfo ? showDefaultAccountInfo() : showLastAccountInfo();
}

void AccountsWidget::onItemClicked(const QModelIndex &index)
{
    m_saveClickedRow = index.row();
    Q_EMIT requestShowAccountsDetail(m_userList[index.row()]);
    m_userlistView->resetStatus(index);
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
        if (titem) {
            titem->setText(user->displayName());
        }
    });
    connect(user, &User::currentAvatarChanged, this, [ = ](const QString & avatar) {
        int tindex = m_userList.indexOf(user);
        auto titem = m_userItemModel->item(tindex);
        if (!titem) {
            return;
        }
        auto ratio = this->devicePixelRatioF();
        auto path = avatar;
        if (ratio > 4.0) {
            path.replace("icons/", "icons/bigger/");
        }

        path = path.startsWith("file://") ? QUrl(path).toLocalFile() : path;
        QPixmap pixmap = pixmapToRound(path);

        titem->setIcon(QIcon(pixmap));
    });
}

QPixmap AccountsWidget::pixmapToRound(const QPixmap &src)
{
    if (src.isNull()) {
        return QPixmap();
    }

    auto pixmap = QPixmap(src);
    QSize size = pixmap.size();
    QPixmap mask(size);
    mask.fill(Qt::transparent);

    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addEllipse(0, 0, size.width(), size.height());
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, pixmap);

    return mask;
}

void AccountsWidget::handleRequestBack(AccountsWidget::ActionOption option)
{
    switch (option) {
    case AccountsWidget::ClickCancel: { //点击取消
        QModelIndex qindex0 = m_userItemModel->index(m_saveClickedRow, 0);
        m_userlistView->setFocus();
        m_userlistView->setCurrentIndex(qindex0);
        onItemClicked(qindex0);
        }
        break;
    case AccountsWidget::CreateUserSuccess: { //创建账户成功
        QModelIndex qindex1 = m_userItemModel->index(m_userItemModel->rowCount() - 1, 0);
        m_userlistView->setFocus();
        m_userlistView->setCurrentIndex(qindex1);
        onItemClicked(qindex1);
        }
        break;
    case AccountsWidget::ModifyPwdSuccess: { //修改密码成功
        QModelIndex qindex2 = m_userItemModel->index(0, 0);
        m_userlistView->setFocus();
        m_userlistView->setCurrentIndex(qindex2);
        onItemClicked(qindex2);
        }
        break;
    }
}
