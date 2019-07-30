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

#include "accountsmodule.h"
#include "createaccountpage.h"
#include "modifypasswdpage.h"
#include "accountswidget.h"
#include "accountsdetailwidget.h"
#include "modules/accounts/accountsworker.h"
#include "modules/accounts/user.h"
#include "modules/accounts/usermodel.h"

#include <QDebug>

using namespace DCC_NAMESPACE::accounts;

AccountsModule::AccountsModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_accountsWidget(nullptr)
{
    m_frameProxy =  frame;
}

void AccountsModule::initialize()
{
    m_userList = new UserModel;
    m_accountsWorker = new AccountsWorker(m_userList);

    m_accountsWorker->moveToThread(qApp->thread());
    m_userList->moveToThread(qApp->thread());

    m_accountsWorker->active();
}

void AccountsModule::reset()
{

}

const QString AccountsModule::name() const
{
    return QStringLiteral("accounts");
}

void AccountsModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName);
}

QWidget *AccountsModule::moduleWidget()
{
    if (!m_accountsWidget) {
        m_accountsWidget = new AccountsWidget;
        m_accountsWidget->setModel(m_userList);
        connect(m_accountsWidget, &AccountsWidget::requestShowAccountsDetail, this, &AccountsModule::onShowAccountsDetailWidget);
        connect(m_accountsWidget, &AccountsWidget::requestCreateAccount, this, &AccountsModule::onShowCreateAccountPage);
    }

    return m_accountsWidget;
}

void AccountsModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w)
}

//显示账户信息
void AccountsModule::onShowAccountsDetailWidget(User *account)
{
    qDebug() << Q_FUNC_INFO;
    AccountsDetailWidget *w = new AccountsDetailWidget(account);

    Q_ASSERT(w);

    connect(w, &AccountsDetailWidget::requestShowPwdSettings, this, &AccountsModule::onShowPasswordPage);
    connect(w, &AccountsDetailWidget::requestSetAutoLogin, m_accountsWorker, &AccountsWorker::setAutoLogin);
    connect(w, &AccountsDetailWidget::requestNopasswdLogin, m_accountsWorker, &AccountsWorker::setNopasswdLogin);
    connect(w, &AccountsDetailWidget::requestDeleteAccount, m_accountsWorker, &AccountsWorker::deleteAccount);
    connect(w, &AccountsDetailWidget::requestChangeFrameAutoHide, this, [&](){
        m_frameProxy->popWidget(this);
    });
    connect(w, &AccountsDetailWidget::requestSetAvatar, m_accountsWorker, &AccountsWorker::setAvatar);
    m_frameProxy->pushWidget(this, w);
}

//创建账户界面
void AccountsModule::onShowCreateAccountPage()
{
    qDebug() << Q_FUNC_INFO;
    CreateAccountPage *w = new CreateAccountPage();
    m_frameProxy->pushWidget(this, w);
}

AccountsModule::~AccountsModule()
{
    m_userList->deleteLater();
    m_accountsWorker->deleteLater();
}

//修改密码界面
void AccountsModule::onShowPasswordPage(User *account)
{
    qDebug() << Q_FUNC_INFO;
    ModifyPasswdPage *w = new ModifyPasswdPage(account);
    connect(w, &ModifyPasswdPage::requestChangePassword, m_accountsWorker, &AccountsWorker::setPassword);
    m_frameProxy->pushWidget(this, w);
}
