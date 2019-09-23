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

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;

LogoutPage::LogoutPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout)
{
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    //~ contents_path /cloudsync/Sorry, it is not supported in your region at present, and will be coming soon
    QLabel *cntip = new QLabel(tr("Sorry, it is not supported in your region at present, and will be coming soon"));
    cntip->setMargin(0);
    cntip->setWordWrap(true);
    cntip->setAlignment(Qt::AlignCenter);

    m_mainLayout->addWidget(cntip, 0, Qt::AlignHCenter);

    QPushButton *logoutBtn = new QPushButton;
    //~ contents_path /cloudsync/Sign Out
    logoutBtn->setText(tr("Sign Out"));
    connect(logoutBtn, &QPushButton::clicked, this, &LogoutPage::requestLogout);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->setSpacing(0);
    bottomLayout->setMargin(0);
    bottomLayout->setContentsMargins(10, 2, 10, 2);
    bottomLayout->addWidget(logoutBtn, 0, Qt::AlignRight);

    m_mainLayout->addLayout(bottomLayout, 1);
    setLayout(m_mainLayout);
}
