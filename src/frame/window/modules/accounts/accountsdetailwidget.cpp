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

using namespace DCC_NAMESPACE::accounts;

AccountsDetailWidget::AccountsDetailWidget(User *user, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)

    , m_modifydelLayout(new QHBoxLayout)
    , m_setloginLayout(new QVBoxLayout)

    , m_mainContentLayout(new QVBoxLayout(this))

    , m_modifydelWidget(new QWidget)
    , m_setloginWidget(new QWidget)

    , m_avatar(new AvatarWidget)
    , m_shortName(new QLabel)
    , m_fullName(new QLabel)
    , m_modifyPassword(new QPushButton)
    , m_deleteAccount(new QPushButton)
    , m_autoLogin(new SwitchWidget)
    , m_nopasswdLogin(new SwitchWidget)
{
    initWidgets();
    initDatas();
}

void AccountsDetailWidget::initWidgets()
{

    m_modifydelLayout->addWidget(m_modifyPassword);
    m_modifydelLayout->addWidget(m_deleteAccount);

    m_setloginLayout->addWidget(m_autoLogin);
    m_setloginLayout->addWidget(m_nopasswdLogin);

    m_modifydelWidget->setLayout(m_modifydelLayout);
    m_setloginWidget->setLayout(m_setloginLayout);

    m_mainContentLayout->addWidget(m_modifydelWidget);
    m_mainContentLayout->addWidget(m_setloginWidget);

}

void AccountsDetailWidget::initDatas()
{
    //use m_curUser fill widget data
    m_modifyPassword->setText(tr("修改密码"));
    m_deleteAccount->setText(tr("删除账户"));

    m_autoLogin->setTitle(tr("自动登录"));
    m_autoLogin->setChecked(m_curUser->autoLogin());
    m_nopasswdLogin->setTitle(tr("无密码登录"));
    m_nopasswdLogin->setChecked(m_curUser->autoLogin());


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
}

//删除账户
void AccountsDetailWidget::deleteUserClicked()
{
//    Q_EMIT requestChangeFrameAutoHide(false);

    RemoveUserDialog d(m_curUser);
    int ret = d.exec();

    if (ret == 1) {

    }

}
