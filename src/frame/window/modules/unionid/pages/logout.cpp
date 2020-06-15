/*
 * Copyright (C) 2017 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     liuxueming <liuxueming@uniontech.com>
 *
 * Maintainer: liuxueming <liuxueming@uniontech.com>
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
#include "modules/unionid/unionidmodel.h"
#include "avatarwidget.h"

#include <DWarningButton>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::unionid;
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
    m_mainLayout->addWidget(m_email, 0, Qt::AlignCenter);
    m_mainLayout->addWidget(m_username, 0, Qt::AlignCenter);
    m_mainLayout->addWidget(cntip, 0, Qt::AlignVCenter);

    setLayout(m_mainLayout);
}

void LogoutPage::setModel(dcc::unionid::UnionidModel *model)
{
    LoginedIn::setModel(model);
    connect(model, &dcc::unionid::UnionidModel::userInfoChanged, this, &LogoutPage::onUserInfoChanged);
    onUserInfoChanged(model->userinfo());
}
