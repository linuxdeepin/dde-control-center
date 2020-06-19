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

#include "index.h"
#include "modules/unionid/unionidmodel.h"

#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "avatarwidget.h"
#include "downloadurl.h"
#include "window/utils.h"

#include <DListView>

#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QDebug>
#include <QDateTime>
#include <QMap>
#include <QDir>

DWIDGET_USE_NAMESPACE

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::unionid;
using namespace dcc::widgets;
using namespace dcc::unionid;

namespace DCC_NAMESPACE {
namespace unionid {

IndexPage::IndexPage(QWidget *parent)
    : LoginedIn(parent)
    , m_mainLayout(new QVBoxLayout)
    , m_listModel(new QStandardItemModel(this))
{
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->addSpacing(100);
    m_mainLayout->addWidget(m_avatar, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(20);
    QHBoxLayout *shortnameLayout = new QHBoxLayout;
    shortnameLayout->setMargin(0);
    shortnameLayout->setAlignment(Qt::AlignHCenter);
    QLabel *inconLabel = new QLabel(this);
    inconLabel->setPixmap(QIcon::fromTheme("dcc_avatar").pixmap(12, 12));

    shortnameLayout->addWidget(inconLabel);
    shortnameLayout->addSpacing(3);
    shortnameLayout->addWidget(m_username);
    QFont font;
    font.setBold(true);
    m_username->setFont(font);
    auto nameWidget = new QWidget();
    nameWidget->setLayout(shortnameLayout);
    m_mainLayout->addWidget(nameWidget, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(m_email, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(15);

    QLabel *passwdlabel = new QLabel();
    passwdlabel->setText(QString("<style> a {text-decoration: none} </style> <a style='color: #0082fa;' href=\"modifypass\">%1</a>").arg(tr("Modify Password")));
    connect(passwdlabel, &QLabel::linkActivated, this, &IndexPage::requesUserDialog);
    QLabel *photolabel = new QLabel();
    photolabel->setText(QString("<style> a {text-decoration: none} </style> <a style='color: #0082fa;' href=\"modifyOic\">%1</a>").arg(tr("Change Avatar")));
    connect(photolabel, &QLabel::linkActivated, this, &IndexPage::requesUserDialog);
    QLabel *userlabel = new QLabel();
    userlabel->setText(QString("<style> a {text-decoration: none} </style> <a style='color: #0082fa;' href=\"UserCenter\">%1</a>").arg(tr("User Center")));
    connect(userlabel, &QLabel::linkActivated, this, &IndexPage::requesUserDialog);
    m_logout = new QLabel();
    m_logout->setText(QString("<style> a {text-decoration: none} </style> <a style='color: #0082fa;' href=\"UserCenter\">%1</a>").arg(tr("Sign Out")));
    connect(m_logout, &QLabel::linkActivated, this, &IndexPage::requestLogout);
    auto modifyWidget = new QWidget();
    QHBoxLayout *modifyLayout = new QHBoxLayout;
    modifyLayout->setSpacing(10);
    modifyLayout->setMargin(0);
    modifyLayout->addWidget(passwdlabel, 0, Qt::AlignHCenter);
    modifyLayout->addWidget(new QLabel("|"), 0, Qt::AlignHCenter);
    modifyLayout->addWidget(photolabel, 0, Qt::AlignHCenter);
    modifyLayout->addWidget(new QLabel("|"), 0, Qt::AlignHCenter);
    modifyLayout->addWidget(userlabel, 0, Qt::AlignHCenter);
    modifyLayout->addSpacing(30);
    modifyLayout->addWidget(new QLabel("|"), 0, Qt::AlignHCenter);
    modifyLayout->addWidget(m_logout, 0, Qt::AlignHCenter);
    modifyLayout->setSizeConstraint(QLayout::SetMinimumSize);
    modifyWidget->setLayout(modifyLayout);

    m_mainLayout->addWidget(modifyWidget, 0, Qt::AlignHCenter);
    m_mainLayout->addStretch(0);
    setLayout(m_mainLayout);
}

void IndexPage::setModel(dcc::unionid::UnionidModel *model)
{
    LoginedIn::setModel(model);
    connect(model, &dcc::unionid::UnionidModel::userInfoChanged, this, &IndexPage::onUserInfoChanged);

    onUserInfoChanged(model->userinfo());
}

IndexPage::~IndexPage()
{
}

}
}
