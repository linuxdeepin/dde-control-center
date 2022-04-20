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
#include "useritemdelegate.h"

#include "accountslistwidget.h"
#include "accountsdetailwidget.h"
#include "src/plugin-accounts/operation/accountsworker.h"
#include "src/plugin-accounts/operation/user.h"
#include "src/plugin-accounts/operation/usermodel.h"
#include "securityquestionspage.h"

#include "src/plugin-accounts/window/useritem.h"
#include "src/plugin-accounts/window/useritempic.h"

#include <DDialog>

#include <QStringList>
#include <QTimer>
#include <QDebug>
#include <QApplication>

#include <QListView>
#include <QListWidget>
#include <QScroller>
#include <QStackedWidget>
#include <QResizeEvent>

DCC_USE_NAMESPACE
QString AccountsPlugin::name() const
{
    return QStringLiteral("accounts");
}

ModuleObject *AccountsPlugin::module()
{
    //一级菜单--账户
    AccountsModule *moduleInterface = new AccountsModule(this);
    ModuleData *dataRoot = new ModuleData(this);
    dataRoot->Name = tr("Accounts");
    dataRoot->DisplayName = tr("Accounts");
    dataRoot->Description = tr("系统账号管理");
    dataRoot->Icon = QIcon::fromTheme("dcc_nav_accounts");
    moduleInterface->setModuleData(dataRoot);
    moduleInterface->setChildType(ModuleObject::ChildType::Page);

    AccountsPageModule *accountsPageModule = new AccountsPageModule(moduleInterface->model(), moduleInterface->work(), moduleInterface);
    moduleInterface->appendChild(accountsPageModule);

    return moduleInterface;
}

AccountsModule::AccountsModule(QObject *parent)
    : ModuleObject(parent)
    , m_model(nullptr)
    , m_worker(nullptr)
{
    m_model  = new UserModel();
    m_worker = new AccountsWorker(m_model);
    m_worker->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());
}

AccountsModule::~AccountsModule()
{
    m_model->deleteLater();
    m_worker->deleteLater();
}

void AccountsModule::active()
{
    m_worker->active();
}

QWidget *AccountsPageModule::page()
{
    m_stackedWidget = new QStackedWidget;
    m_page = new QWidget;
    m_mainContentLayout = new QVBoxLayout();
    m_accountsListWidget = new AccountsListWidget;
    m_accountsListWidget->setModel(m_model);
    m_accountsListWidget->setShowFirstUserInfo(true);

    connect(m_accountsListWidget, &AccountsListWidget::requestShowAccountsDetail, this, [&](User *currUser)
    {
        m_mainContentLayout->removeWidget(m_accountsDetailWidget);
        if(m_accountsDetailWidget)
            m_accountsDetailWidget->deleteLater();

        m_accountsDetailWidget = updateAccountsDetailWidget(currUser);
        m_mainContentLayout->insertWidget(1, m_accountsDetailWidget);
        m_accountsDetailWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    });
    connect(m_accountsListWidget, &AccountsListWidget::requestCreateAccount, this, &AccountsPageModule::onShowCreateAccountPage);
    connect(m_accountsListWidget, &AccountsListWidget::requestLoadUserList, m_worker, &AccountsWorker::loadUserList);
    connect(m_accountsListWidget, &AccountsListWidget::requestUpdatGroupList, m_worker, &AccountsWorker::updateGroupinfo);
    connect(m_accountsListWidget, &AccountsListWidget::requestBack, this, [ = ] {
        m_stackedWidget->setCurrentIndex(0);
    });
    m_accountsListWidget->showDefaultAccountInfo();

    m_mainContentLayout->setMargin(0);
    m_mainContentLayout->insertWidget(0, m_accountsListWidget);
    m_page->setLayout(m_mainContentLayout);

    m_stackedWidget->insertWidget(0,m_page);
    m_stackedWidget->insertWidget(1,new QWidget()); //占位：修改密码
    m_stackedWidget->insertWidget(2,new QWidget()); //占位：创建用户
    m_stackedWidget->insertWidget(3,new QWidget()); //占位：安全问题
    m_stackedWidget->setCurrentWidget(m_page);
    return m_stackedWidget;
}

void AccountsPageModule::deactive()
{
    m_page = nullptr;
    m_accountsListWidget = nullptr;
    m_accountsDetailWidget = nullptr;
    m_stackedWidget = nullptr;
}

//创建账户界面
void AccountsPageModule::onShowCreateAccountPage()
{
    CreateAccountPage *w = new CreateAccountPage(m_worker);
    User *newUser = new User(this);
    w->setModel(m_model, newUser);
    connect(w, &CreateAccountPage::requestCreateUser, m_worker, &AccountsWorker::createAccount);
    connect(m_worker, &AccountsWorker::accountCreationFinished, w, &CreateAccountPage::setCreationResult);
    connect(w, &CreateAccountPage::requestBack, m_accountsListWidget, &AccountsListWidget::handleRequestBack);
    m_accountsListWidget->setShowDefaultAccountInfo(false);

    m_stackedWidget->removeWidget(m_stackedWidget->widget(2));
    m_stackedWidget->insertWidget(2, w);
    m_stackedWidget->setCurrentIndex(2);
}

//显示账户信息
AccountsDetailWidget * AccountsPageModule::updateAccountsDetailWidget(User *account)
{
    AccountsDetailWidget *w = new AccountsDetailWidget(account, m_model);
    w->setVisible(false);

    connect(m_model, &UserModel::deleteUserSuccess, w, &AccountsDetailWidget::requestBack);
    connect(m_model, &UserModel::isCancelChanged, w, &AccountsDetailWidget::resetDelButtonState);
    connect(w, &AccountsDetailWidget::requestShowPwdSettings, this, &AccountsPageModule::onShowPasswordPage);
    connect(w, &AccountsDetailWidget::requestShowSecurityQuestionsSettings, this, &AccountsPageModule::onShowSecurityQuestionsPage);
    connect(w, &AccountsDetailWidget::requestSetAutoLogin, m_worker, &AccountsWorker::setAutoLogin);
    connect(w, &AccountsDetailWidget::requestSetAdministrator, m_worker, &AccountsWorker::setAdministrator);
    connect(w, &AccountsDetailWidget::requestNopasswdLogin, m_worker, &AccountsWorker::setNopasswdLogin);
    connect(w, &AccountsDetailWidget::requestDeleteAccount, m_worker, &AccountsWorker::deleteAccount);
    connect(w, &AccountsDetailWidget::requestSetGroups, m_worker, &AccountsWorker::setGroups);
    connect(w, &AccountsDetailWidget::requestBack, this, [&]() {
        m_accountsListWidget->setShowFirstUserInfo(false);
    });
    connect(w, &AccountsDetailWidget::requestSetAvatar, m_worker, &AccountsWorker::setAvatar);
    connect(w, &AccountsDetailWidget::requestSetFullname, m_worker, &AccountsWorker::setFullname);
    connect(w, &AccountsDetailWidget::requsetSetPassWordAge, m_worker, &AccountsWorker::setMaxPasswordAge);
    connect(w, &AccountsDetailWidget::editingFinished, this, [ = ](QString userFullName) {
        QDBusPendingReply<bool, QString, int> reply = m_worker->isUsernameValid(userFullName);
        //欧拉版会自己创建shutdown等root组账户且不会添加到userList中，导致无法重复性算法无效，先通过isUsernameValid校验这些账户再通过重复性算法校验
        //vaild == false && code ==6 是用户名已存在
        if (!reply.argumentAt(0).toBool() && PassErrorCode::ErrCodeSystemUsed == reply.argumentAt(2).toInt()) {
            w->onEditingFinished(true, userFullName);
        } else {
            w->onEditingFinished(false, userFullName);
        }
    });
    connect(w, &AccountsDetailWidget::requestSecurityQuestionsCheck, m_worker, &AccountsWorker::asyncSecurityQuestionsCheck);

    if (account->isCurrentUser()) {
        Q_EMIT w->requestSecurityQuestionsCheck(account);
    }
    w->setVisible(true);
    //当前页面为用户详情页面的时候允许跳转默认帐户，否则只响应用户点击。避免出现显示了创建账户页面，又被账户详情页面隐藏的问题。
    m_accountsListWidget->setShowDefaultAccountInfo(true);
    return w;
}

//修改密码界面
void AccountsPageModule::onShowPasswordPage(User *account)
{
    ModifyPasswdPage *w = new ModifyPasswdPage(account, account->isCurrentUser());

    connect(w, &ModifyPasswdPage::requestChangePassword, m_worker, &AccountsWorker::setPassword);
    connect(w, &ModifyPasswdPage::requestResetPassword, m_worker, &AccountsWorker::resetPassword);
    connect(w, &ModifyPasswdPage::requestBack, m_accountsListWidget, &AccountsListWidget::handleRequestBack);
    connect(w, &ModifyPasswdPage::requestSetPasswordHint, m_worker, &AccountsWorker::setPasswordHint);
    connect(w, &ModifyPasswdPage::requestUOSID, m_worker, &AccountsWorker::getUOSID);
    connect(w, &ModifyPasswdPage::requestUUID, m_worker, &AccountsWorker::getUUID);
    connect(w, &ModifyPasswdPage::requestLocalBindCheck, m_worker, &AccountsWorker::localBindCheck);
    connect(w, &ModifyPasswdPage::requestStartResetPasswordExec, m_worker, &AccountsWorker::startResetPasswordExec);
    connect(w, &ModifyPasswdPage::requestSecurityQuestionsCheck, m_worker, &AccountsWorker::asyncSecurityQuestionsCheck);
    connect(m_worker, &AccountsWorker::localBindUbid, w, &ModifyPasswdPage::onLocalBindCheckUbid);
    connect(m_worker, &AccountsWorker::localBindError, w, &ModifyPasswdPage::onLocalBindCheckError);

    m_stackedWidget->removeWidget(m_stackedWidget->widget(1));
    m_stackedWidget->insertWidget(1, w);
    m_stackedWidget->setCurrentIndex(1);
}

void AccountsPageModule::onShowSecurityQuestionsPage(User *account)
{
    SecurityQuestionsPage *w = new SecurityQuestionsPage(account);
    w->setVisible(false);
    connect(w, &SecurityQuestionsPage::requestBack, m_accountsListWidget, &AccountsListWidget::handleRequestBack);
    connect(w, &SecurityQuestionsPage::requestSecurityQuestionsCheck, m_worker, &AccountsWorker::asyncSecurityQuestionsCheck);
    connect(w, &SecurityQuestionsPage::requestSetSecurityQuestions, m_worker, &AccountsWorker::setSecurityQuestions);

    Q_EMIT w->requestSecurityQuestionsCheck(account);

    w->setVisible(true);

    m_stackedWidget->removeWidget(m_stackedWidget->widget(3));
    m_stackedWidget->insertWidget(3, w);
    m_stackedWidget->setCurrentIndex(3);
}
