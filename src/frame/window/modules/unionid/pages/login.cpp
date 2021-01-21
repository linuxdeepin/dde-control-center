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

#include "login.h"

#include <DSuggestButton>
#include <DFontSizeManager>
#include <DTipLabel>
#include <DSysInfo>

#include <QVBoxLayout>
#include <QLabel>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::unionid;
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

#include "window/utils.h"
namespace DCC_NAMESPACE {
namespace unionid {
QString systemName()
{
    if (SystemTypeName == "Professional")
        return "UOS";

    return "Deepin";
}
}
}
LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout)
{
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    //~ contents_path /cloudsync/Sync ID Sign In
    DSuggestButton *loginBtn = new DSuggestButton(tr("Sign In"));
    loginBtn->setMinimumWidth(200);
    DTipLabel *tip = new DTipLabel(tr("After signing in, all system features related with Union ID will be available.\n"
                                      "Visit User Center and manage your account there."));
    tip->setWordWrap(true);
    tip->setAlignment(Qt::AlignCenter);

    m_mainLayout->addStretch(3);
    QLabel *pLogo = new QLabel;
    pLogo->setPixmap(QIcon::fromTheme("dcc_unionid").pixmap(128, 100));
    m_mainLayout->addWidget(pLogo, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(10);
    QLabel *signintip = new QLabel(tr("Union ID Sign In"));
    QFont font = signintip->font();
    font.setBold(true);
    signintip->setFont(font);
    DFontSizeManager::instance()->bind(signintip, DFontSizeManager::T3);

    m_mainLayout->addWidget(signintip, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(6);
    m_mainLayout->addWidget(tip);
    m_mainLayout->addSpacing(35);
    m_mainLayout->addWidget(loginBtn, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(20);
    m_mainLayout->addStretch(5);
    connect(loginBtn, &QPushButton::clicked, this, &LoginPage::requestLoginUser);
    setLayout(m_mainLayout);
}

