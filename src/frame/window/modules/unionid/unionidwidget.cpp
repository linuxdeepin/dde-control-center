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
#include "modules/unionid/httpclient.h"
#include "define.h"

#include <QVBoxLayout>
#include <QStackedLayout>
#include <QLabel>
#include <QNetworkReply>
#include <DFontSizeManager>
#include <QDBusInterface>

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

//    connect(m_loginPage, &LoginPage::requestSignInUser, this, &UnionidWidget::requestSignInUser);
    connect(m_loginPage, &LoginPage::requestLoginUser, this, &UnionidWidget::requestLoginUser);
    connect(m_indexPage, &IndexPage::requestSetAutoSync, this, &UnionidWidget::requestSetAutoSync);
//    connect(m_indexPage, &IndexPage::requestLogout, this, &UnionidWidget::requestLogoutUser);
    connect(m_indexPage, &IndexPage::requestLogout, this, &UnionidWidget::onRequestLogout);
    connect(m_indexPage, &IndexPage::requestSetModuleState, this, &UnionidWidget::requestSetModuleState);
    connect(m_indexPage, &IndexPage::requesUserDialog, this, &UnionidWidget::requesUserDialog);
    connect(m_cnonlyPage, &LogoutPage::requestLogout, this, &UnionidWidget::requestLogoutUser);

//    QLabel *label = new QLabel();
    //    label->setText(tr("Learn about %1 and %2").arg(QString("<style> a {text-decoration: none} </style> <a style='color: #0082fa;' href=\"servicelabel\"> %1 </a>")
    //                                              .arg(tr("Union ID Service Agreement"))).arg(QString("<style> a {text-decoration: none} </style> <a style='color: #0082fa;' href=\"privacyLabel\"> %1 </a>")
    //                                              .arg(tr("Privacy Policy"))));
    //    DFontSizeManager::instance()->bind(label, DFontSizeManager::T8);
    //    connect(label, &QLabel::linkActivated, this, &UnionidWidget::requestPopupDialog)0000;

//    QHBoxLayout *hyperlinksLayout = new QHBoxLayout();
//    hyperlinksLayout->addWidget(label, 0, Qt::AlignHCenter);
//    auto linksandLogoutwidhet = new QWidget;
//    linksandLogoutwidhet->setLayout(hyperlinksLayout);
//    mainLayout->addWidget(linksandLogoutwidhet);
    setLayout(m_pageLayout);
    qInfo() << "rect" << rect();
}

void UnionidWidget::setModel(dcc::unionid::UnionidModel *model, MainWindow *pMainWindow)
{
    m_model = model;
    m_indexPage->setModel(model);
    m_cnonlyPage->setModel(model);
    m_loginPage->setMainWindow(pMainWindow);

    connect(model, &dcc::unionid::UnionidModel::userInfoChanged, this, &UnionidWidget::onUserInfoChanged);

    onUserInfoChanged(model->userinfo());
}

void UnionidWidget::getAccessToken(const QString &code, const QString &state)
{
    Q_UNUSED(state)
    QNetworkReply *reply = HttpClient::instance()->getAccessToken(CLIENT_ID,code);
    connect(reply,&QNetworkReply::finished,this,&UnionidWidget::onGetAccessToken);
}

void UnionidWidget::onGetAccessToken()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    QString result = HttpClient::instance()->checkReply(reply);

    qInfo() << "solveJson" << HttpClient::instance()->solveJson(result);
    if (HttpClient::instance()->solveJson(result)) {
        m_indexPage->setUserInfo(result);
//        RequestService::instance()->getBoundAccount()
        m_pageLayout->setCurrentWidget(m_indexPage);
    }
}

void UnionidWidget::onRequestLogout()
{
    QDBusInterface interface("com.deepin.deepinid.Client",
                              "/com/deepin/deepinid/Client",
                              "com.deepin.deepinid.Client");

    QDBusMessage msg = interface.call(QDBus::Block, "ConfirmLogout");

    if (msg.type() == QDBusMessage::ReplyMessage) {
        bool bIsSuccess = msg.arguments().takeFirst().toBool();

        if (bIsSuccess) {
            m_pageLayout->setCurrentWidget(m_loginPage);
        } else {
            qInfo() << "退出登录失败";
        }
    }
}

void UnionidWidget::onUserInfoChanged(const QVariantMap &userInfo)
{
    const bool isLogind = !userInfo["Username"].toString().isEmpty();
    const QString region = userInfo["Region"].toString();

    if (isLogind) {
        if (region == "CN") {
            m_loginPage->login();
//            m_pageLayout->setCurrentWidget(m_indexPage);
        } else {
            m_pageLayout->setCurrentWidget(m_cnonlyPage);
        }
    }
    else {
        m_indexPage->requestLogout();
        //m_pageLayout->setCurrentWidget(m_loginPage);
    }
}
