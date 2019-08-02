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

#include "accountsdetailwidget.h"
#include "modules/accounts/removeuserdialog.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QLineEdit>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

AccountsDetailWidget::AccountsDetailWidget(User *user, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
    , m_headLayout(new QVBoxLayout)
    , m_modifydelLayout(new QHBoxLayout)
    , m_setloginLayout(new QVBoxLayout)
    , m_mainContentLayout(new QVBoxLayout(this))
    , m_fullnameLayout(new QHBoxLayout)
    , m_inputlineLayout(new QHBoxLayout)
    , m_avatar(new AvatarWidget)
    , m_shortName(new QLabel)
    , m_fullName(new QLabel)
    , m_inputLineEdit(new QLineEdit)
    , m_modifyPassword(new QPushButton)
    , m_deleteAccount(new QPushButton)
    , m_autoLogin(new SwitchWidget)
    , m_nopasswdLogin(new SwitchWidget)
    , m_avatarListWidget(new AvatarListWidget)
    , m_fullnameBtn(new DImageButton)
    , m_inputeditBtn(new DImageButton)
{
    initWidgets();
    initDatas();
}

void AccountsDetailWidget::initWidgets()
{
    setLayout(m_mainContentLayout);

    m_fullnameBtn->setNormalPic(":/widgets/themes/dark/icons/edit_normal@2x.png");
    m_fullnameBtn->setHoverPic(":/widgets/themes/dark/icons/edit_hover@2x.png");
    m_fullnameBtn->setPressPic(":/widgets/themes/dark/icons/edit_press@2x.png");

    m_inputeditBtn->setNormalPic(":/widgets/themes/dark/icons/edit_normal@2x.png");
    m_inputeditBtn->setHoverPic(":/widgets/themes/dark/icons/edit_hover@2x.png");
    m_inputeditBtn->setPressPic(":/widgets/themes/dark/icons/edit_press@2x.png");

    m_headLayout->addWidget(m_avatar, 0, Qt::AlignHCenter);
    m_headLayout->addWidget(m_shortName, 0, Qt::AlignHCenter);
    m_headLayout->addLayout(m_fullnameLayout);
    m_headLayout->addLayout(m_inputlineLayout);

    m_fullnameLayout->addWidget(m_fullName, 0, Qt::AlignHCenter);
    m_fullnameLayout->addWidget(m_fullnameBtn, 0, Qt::AlignHCenter);

    m_inputlineLayout->addWidget(m_inputLineEdit, 0, Qt::AlignHCenter);
    m_inputlineLayout->addWidget(m_inputeditBtn, 0, Qt::AlignHCenter);

    m_modifydelLayout->addWidget(m_modifyPassword);
    m_modifydelLayout->addWidget(m_deleteAccount);

    m_setloginLayout->addWidget(m_autoLogin);
    m_setloginLayout->addWidget(m_nopasswdLogin);

    m_mainContentLayout->addLayout(m_headLayout);
    m_mainContentLayout->addLayout(m_modifydelLayout);
    m_mainContentLayout->addLayout(m_setloginLayout);
    m_mainContentLayout->addStretch();

    m_shortName->setFixedHeight(20);
    m_fullName->setFixedHeight(20);
    m_avatar->setAlignment(Qt::AlignHCenter);
    m_setloginLayout->setContentsMargins(0, 0, 0, 0);
    m_setloginLayout->setSpacing(0);
    m_setloginLayout->setMargin(0);

    m_modifydelLayout->setContentsMargins(1, 0, 1, 0);
    m_modifydelLayout->setSpacing(10);
    m_modifydelLayout->setMargin(3);

    const bool isOnline = m_curUser->online();
    m_deleteAccount->setDisabled(isOnline);

    m_inputLineEdit->setVisible(false);
    m_inputeditBtn->setVisible(false);
}

void AccountsDetailWidget::initDatas()
{
    connect(m_curUser, &User::autoLoginChanged, m_autoLogin, &SwitchWidget::setChecked);
    connect(m_curUser, &User::nopasswdLoginChanged, m_nopasswdLogin, &SwitchWidget::setChecked);

    connect(m_deleteAccount, &QPushButton::clicked, this, &AccountsDetailWidget::deleteUserClicked);

    connect(m_modifyPassword, &QPushButton::clicked, [ = ] {
        Q_EMIT requestShowPwdSettings(m_curUser);
    });

    connect(m_autoLogin, &SwitchWidget::checkedChanged, [ = ](const bool autoLogin) {
        Q_EMIT requestSetAutoLogin(m_curUser, autoLogin);
    });

    connect(m_nopasswdLogin, &SwitchWidget::checkedChanged, [ = ](const bool nopasswdLogin) {
        Q_EMIT requestNopasswdLogin(m_curUser, nopasswdLogin);
    });

    connect(m_avatar, &AvatarWidget::clicked, this, [ = ](const QString & iconPath) {
        Q_UNUSED(iconPath)
        m_modifyPassword->setVisible(false);
        m_deleteAccount->setVisible(false);
        m_autoLogin->setVisible(false);
        m_nopasswdLogin->setVisible(false);
        m_modifydelLayout->addWidget(m_avatarListWidget);
    });

    connect(m_avatarListWidget, &AvatarListWidget::requestSetAvatar, this, [ = ](const QString & avatarPath) {
        Q_EMIT requestSetAvatar(m_curUser, avatarPath);
    });

    connect(m_curUser, &User::currentAvatarChanged, m_avatar, &AvatarWidget::setAvatarPath);

    connect(m_curUser, &User::fullnameChanged, m_fullName, &QLabel::setText);

    connect(m_fullnameBtn, &DImageButton::clicked, this, &AccountsDetailWidget::updateLineEditDisplayStyle);

    connect(m_inputeditBtn, &DImageButton::clicked, this, [ = ]() {
        Q_EMIT requestShowFullnameSettings(m_curUser, m_inputLineEdit->text());
        updateLineEditDisplayStyle();
    });

    //use m_curUser fill widget data
    m_avatar->setAvatarPath(m_curUser->currentAvatar());
    m_shortName->setText(m_curUser->name());
    m_fullName->setText(m_curUser->fullname());

    m_modifyPassword->setText(tr("修改密码"));
    m_deleteAccount->setText(tr("删除账户"));

    m_autoLogin->setTitle(tr("自动登录"));
    m_autoLogin->setChecked(m_curUser->autoLogin());
    m_nopasswdLogin->setTitle(tr("无密码登录"));
    m_nopasswdLogin->setChecked(m_curUser->autoLogin());
}

void AccountsDetailWidget::updateLineEditDisplayStyle()
{
    const bool visible = m_inputLineEdit->isVisible();

    m_fullName->setVisible(visible);
    m_fullnameBtn->setVisible(visible);
    m_inputLineEdit->setVisible(!visible);;
    m_inputeditBtn->setVisible(!visible);

    if (!visible) {
        m_inputLineEdit->setText(m_fullName->text());
        m_inputLineEdit->selectAll();
    }
}

//删除账户
void AccountsDetailWidget::deleteUserClicked()
{
    RemoveUserDialog d(m_curUser);
    int ret = d.exec();

    if (ret == 1) {
        Q_EMIT requestDeleteAccount(m_curUser, d.deleteHome());
        Q_EMIT requestBack();
    }
}
