/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
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

#include "useroptionitem.h"
#include "widgets/nextpagewidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsheaderitem.h"

#include <QPushButton>
#include <QDebug>

using namespace dcc::widgets;
using namespace dcc::accounts;

AccountsWidget::AccountsWidget()
    : ModuleWidget()
    , m_userGroup(new dcc::widgets::SettingsGroup)
    , m_createBtn(new QPushButton(tr("Create Account")))
#ifdef DCC_ENABLE_ADDOMAIN
    , m_adBtn(new QPushButton)
#endif
{
    setObjectName("Accounts");

    m_centralLayout->addWidget(m_userGroup);
    m_centralLayout->addWidget(m_createBtn);

#ifdef DCC_ENABLE_ADDOMAIN
    m_centralLayout->addWidget(m_adBtn);
#endif

    setTitle(tr("Accounts"));

    connect(m_createBtn, &QPushButton::clicked, this, &AccountsWidget::requestCreateAccount);
#ifdef DCC_ENABLE_ADDOMAIN
    connect(m_adBtn, &QPushButton::clicked, this, &AccountsWidget::requestShowADDialog);
#endif
}

#ifdef DCC_ENABLE_ADDOMAIN
void AccountsWidget::setModel(UserModel * const model)
{
    m_model = model;

    connect(model, &UserModel::isADUserLoginChanged, m_adBtn, &QPushButton::setDisabled);

    m_adBtn->setDisabled(model->isADUserLogind());
}
#endif

void AccountsWidget::addUser(User *user)
{
    UserOptionItem *w = new UserOptionItem;

    m_userGroup->appendItem(w);

    auto setName = [=] {
        w->setTitle(user->displayName());
    };

    connect(user, &User::nameChanged, this, setName);
    connect(user, &User::fullnameChanged, this, setName);
    connect(user, &User::currentAvatarChanged, w, &UserOptionItem::setAvatar);
    connect(user, &User::destroyed, this, [=] { m_userGroup->removeItem(w); w->deleteLater(); });
    connect(w, &NextPageWidget::clicked, [=] { Q_EMIT showAccountsDetail(user); });

    setName();
    w->setAvatar(user->currentAvatar());
}

#ifdef DCC_ENABLE_ADDOMAIN
void AccountsWidget::setADState(bool isJoin)
{
    m_adBtn->setText(isJoin ? tr("Leave domain") : tr("Join domain"));
}

void AccountsWidget::showEvent(QShowEvent *event)
{
    ModuleWidget::showEvent(event);

    Q_EMIT requestRefreshADState();
}
#endif
