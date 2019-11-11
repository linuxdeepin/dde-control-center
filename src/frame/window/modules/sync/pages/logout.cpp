/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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
#include "logout.h"
#include "modules/sync/syncmodel.h"
#include "avatarwidget.h"

#include <DWarningButton>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;
DWIDGET_USE_NAMESPACE

LogoutPage::LogoutPage(QWidget *parent)
    : LoginedIn(parent)
    , m_mainLayout(new QVBoxLayout)
{
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    QLabel *cntip = new QLabel(tr("Sorry, it is not supported in your region at present, and will be coming soon"));
    cntip->setMargin(0);
    cntip->setWordWrap(true);
    cntip->setAlignment(Qt::AlignCenter);

    m_mainLayout->addStretch(2);
    m_mainLayout->addWidget(m_avatar, 0, Qt::AlignCenter);
    m_mainLayout->addWidget(m_username, 0, Qt::AlignCenter);
    m_mainLayout->addWidget(cntip, 0, Qt::AlignVCenter);

    DWarningButton *logoutBtn = new DWarningButton;
    logoutBtn->setText(tr("Sign Out"));
    connect(logoutBtn, &QPushButton::clicked, this, &LogoutPage::requestLogout);
    m_mainLayout->addSpacing(35);
    m_mainLayout->addWidget(logoutBtn, 0, Qt::AlignCenter);
    m_mainLayout->addStretch(3);

    setLayout(m_mainLayout);
}

void LogoutPage::setModel(dcc::cloudsync::SyncModel *model)
{
    LoginedIn::setModel(model);
    connect(model, &dcc::cloudsync::SyncModel::userInfoChanged, this, &LogoutPage::onUserInfoChanged);
    onUserInfoChanged(model->userinfo());
}
