/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "useroptionitem.h"
#include "switchwidget.h"
#include "settingsheaderitem.h"

#include <QPushButton>
#include <QDebug>

using namespace dcc::widgets;
using namespace dcc::accounts;

AccountsWidget::AccountsWidget()
    : ModuleWidget(),

    m_userGroup(new dcc::widgets::SettingsGroup),
    m_createBtn(new QPushButton(tr("Create Account")))
{
    setObjectName("Accounts");

    m_centralLayout->addWidget(m_userGroup);
    m_centralLayout->addWidget(m_createBtn);

    setTitle(tr("Accounts"));

    connect(m_createBtn, &QPushButton::clicked, this, &AccountsWidget::requestCreateAccount);
}

void AccountsWidget::addUser(User *user)
{
    UserOptionItem *w = new UserOptionItem;

    m_userGroup->appendItem(w);

    connect(user, &User::nameChanged, w, &NextPageWidget::setTitle);
    connect(user, &User::currentAvatarChanged, w, &UserOptionItem::setAvatar);
    connect(w, &NextPageWidget::clicked, [=] { emit showAccountsDetail(user); });

    w->setTitle(user->name());
    w->setAvatar(user->currentAvatar());
}

void AccountsWidget::removeUser(User *user)
{
    QList<NextPageWidget *> items = findChildren<NextPageWidget*>();
    for (NextPageWidget *item : items) {
        if (item->title() == user->name()) {
            m_userGroup->removeItem(item);
            item->deleteLater();
            break;
        }
    }
}
