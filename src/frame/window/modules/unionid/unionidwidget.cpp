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

#include "unionidwidget.h"
#include "pages/login.h"
#include "pages/index.h"
#include "pages/logout.h"
#include "modules/unionid/unionidmodel.h"

#include <QVBoxLayout>
#include <QStackedLayout>
#include <QLabel>
#include <DWarningButton>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::unionid;
DWIDGET_USE_NAMESPACE

UnionidWidget::UnionidWidget(QWidget *parent)
    : QWidget(parent)
    , m_pageLayout(new QStackedLayout)
    , m_model(nullptr)
    , m_loginPage(new LoginPage)
    , m_indexPage(new IndexPage)
    , m_cnonlyPage(new LogoutPage)
{
    m_pageLayout->setMargin(0);
    m_pageLayout->setSpacing(0);

    m_indexPage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pageLayout->addWidget(m_loginPage);
    m_pageLayout->addWidget(m_indexPage);
    m_pageLayout->addWidget(m_cnonlyPage);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(m_pageLayout);
    connect(m_loginPage, &LoginPage::requestLoginUser, this, &UnionidWidget::requestLoginUser);
    connect(m_indexPage, &IndexPage::requestSetAutoSync, this, &UnionidWidget::requestSetAutoSync);
    connect(m_indexPage, &IndexPage::requestLogout, this, &UnionidWidget::requestLogoutUser);
    connect(m_indexPage, &IndexPage::requesUserDialog, this, &UnionidWidget::requesUserDialog);
    connect(m_cnonlyPage, &LogoutPage::requestLogout, this, &UnionidWidget::requestLogoutUser);
    QLabel *label1 = new QLabel(tr("Learn about"));
    QLabel *servicelabel = new QLabel();
    servicelabel->setText(QString("<style> a {text-decoration: none} </style> <a style='color: blue;' href=\"servicelabel\">《%1》</a>")
                          .arg("UnionId Account service agreements"));
    connect(servicelabel, &QLabel::linkActivated, this, &UnionidWidget::requestPopupDialog);
    QLabel *label2 = new QLabel(tr("and"));

    QLabel  *privacyLabel = new QLabel();
    privacyLabel->setText(QString("<style> a {text-decoration: none} </style> <a style='color: blue;' href=\"privacyLabel\">《%1》</a>")
                          .arg("UOS Privacy Policy"));
    connect(privacyLabel, &QLabel::linkActivated, this, &UnionidWidget::requestPopupDialog);
    auto linkWidhet = new QWidget;
    QHBoxLayout *hyperlinksLayout = new QHBoxLayout();
    hyperlinksLayout->addSpacing(0);
    hyperlinksLayout->addWidget(label1, 0, Qt::AlignHCenter);
    hyperlinksLayout->addWidget(servicelabel, 0, Qt::AlignHCenter);
    hyperlinksLayout->addWidget(label2, 0, Qt::AlignHCenter);
    hyperlinksLayout->addWidget(privacyLabel, 0, Qt::AlignHCenter);
    m_logoutBtn = new DWarningButton;
    m_logoutBtn->setText(tr("Sign Out"));
    linkWidhet->setLayout(hyperlinksLayout);
    QHBoxLayout *linksandLogoutLayout = new QHBoxLayout();
    m_label = new QLabel;
    linksandLogoutLayout->addWidget(m_label, 0, Qt::AlignLeft);
    linksandLogoutLayout->addWidget(linkWidhet, 0, Qt::AlignCenter);
    linksandLogoutLayout->addWidget(m_logoutBtn, 0, Qt::AlignRight);
    connect(m_logoutBtn, &QPushButton::clicked, this, &UnionidWidget::requestLogoutUser);
    auto linksandLogoutwidhet = new QWidget;
    linksandLogoutwidhet->setLayout(linksandLogoutLayout);
    mainLayout->addWidget(linksandLogoutwidhet);
    setLayout(mainLayout);
}

void UnionidWidget::setModel(dcc::unionid::UnionidModel *model)
{
    m_model = model;
    m_indexPage->setModel(model);
    m_cnonlyPage->setModel(model);

    connect(model, &dcc::unionid::UnionidModel::userInfoChanged, this, &UnionidWidget::onUserInfoChanged);

    onUserInfoChanged(model->userinfo());
}

void UnionidWidget::onUserInfoChanged(const QVariantMap &userInfo)
{
    const bool isLogind = !userInfo["Username"].toString().isEmpty();
    //const QString region = userInfo["Region"].toString();

    if (isLogind) {
        m_label->show();
        m_logoutBtn->show();
        m_pageLayout->setCurrentWidget(m_indexPage);
        /*else {
        m_pageLayout->setCurrentWidget(m_cnonlyPage);
        }*/
    } else {
        m_label->hide();
        m_logoutBtn->hide();
        m_pageLayout->setCurrentWidget(m_loginPage);
    }
}
