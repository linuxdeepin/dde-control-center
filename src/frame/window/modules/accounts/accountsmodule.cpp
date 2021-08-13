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
#include "modules/accounts/fingerworker.h"
#include "modules/accounts/fingermodel.h"
#include "addfingedialog.h"

#include <DDialog>

#include <QStringList>
#include <QTimer>
#include <QDebug>

using namespace dcc::accounts;
using namespace DCC_NAMESPACE::accounts;

AccountsModule::AccountsModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_isCreatePage(false)
{
    m_frameProxy =  frame;
    m_pMainWindow = dynamic_cast<MainWindow *>(m_frameProxy);
}

void AccountsModule::initialize()
{
    if (m_userModel) {
        delete m_userModel;
    }
    m_userModel = new UserModel(this);
    m_accountsWorker = new AccountsWorker(m_userModel);

    m_accountsWorker->moveToThread(qApp->thread());
    m_userModel->moveToThread(qApp->thread());

    m_fingerModel = new FingerModel(this);
    m_fingerWorker = new FingerWorker(m_fingerModel);

    m_fingerModel->moveToThread(qApp->thread());
    m_fingerWorker->moveToThread(qApp->thread());

    m_accountsWorker->active();
    connect(m_fingerModel, &FingerModel::vaildChanged, this, &AccountsModule::onHandleVaildChanged);
    connect(m_accountsWorker, &AccountsWorker::requestMainWindowEnabled, this, &AccountsModule::onSetMainWindowEnabled);
}

void AccountsModule::reset()
{

}

const QString AccountsModule::name() const
{
    return QStringLiteral("accounts");
}

const QString AccountsModule::displayName() const
{
    return tr("Accounts");
}

void AccountsModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName);
}

void AccountsModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w)
}

void AccountsModule::active()
{
    m_accountsWidget = new AccountsWidget;
    m_accountsWidget->setVisible(false);
    m_accountsWidget->setModel(m_userModel);
    m_accountsWidget->setShowFirstUserInfo(true);
    connect(m_accountsWidget, &AccountsWidget::requestShowAccountsDetail, this, &AccountsModule::onShowAccountsDetailWidget);
    connect(m_accountsWidget, &AccountsWidget::requestCreateAccount, this, &AccountsModule::onShowCreateAccountPage);
    connect(m_accountsWidget, &AccountsWidget::requestBack, this, [ = ] {
        m_frameProxy->popWidget(this);
    });
    connect(m_accountsWidget, &AccountsWidget::requestLoadUserList, m_accountsWorker, &AccountsWorker::loadUserList);
    connect(m_accountsWidget, &AccountsWidget::requestUpdatGroupList, m_accountsWorker, &AccountsWorker::updateGroupinfo);
    m_frameProxy->pushWidget(this, m_accountsWidget);
    m_accountsWidget->setVisible(true);
    m_accountsWidget->showDefaultAccountInfo();
}

int AccountsModule::load(const QString &path)
{
    if (!m_accountsWidget) {
        active();
    }

    QStringList searchList;
    searchList << "Accounts Detail"
               << "New Account";

    User *pUser = nullptr;
    for (auto &user : m_userModel->userList()) {
        if (user->isCurrentUser()) {
            pUser = user;
            break;
        }
    }

    if (path == searchList[0] && pUser) {
        onShowAccountsDetailWidget(pUser);
        return 0;
    } else if (path == searchList[1]) {
        onShowCreateAccountPage();
        return 0;
    }

    return -1;
}

QStringList AccountsModule::availPage() const
{
    QStringList availList;
    availList << "Accounts Detail"
              << "New Account";
    return availList;
}

//显示账户信息
void AccountsModule::onShowAccountsDetailWidget(User *account)
{
    AccountsDetailWidget *w = new AccountsDetailWidget(account);
    w->setVisible(false);
    w->setAccountModel(m_userModel);
    m_fingerWorker->refreshUserEnrollList(account->name());
    w->setFingerModel(m_fingerModel);

    connect(m_userModel, &UserModel::deleteUserSuccess, w, &AccountsDetailWidget::requestBack);
    connect(m_userModel, &UserModel::isCancelChanged, w, &AccountsDetailWidget::resetDelButtonState);
    connect(w, &AccountsDetailWidget::requestShowPwdSettings, this, &AccountsModule::onShowPasswordPage);
    connect(w, &AccountsDetailWidget::requestSetAutoLogin, m_accountsWorker, &AccountsWorker::setAutoLogin);
    connect(w, &AccountsDetailWidget::requestNopasswdLogin, m_accountsWorker, &AccountsWorker::setNopasswdLogin);
    connect(w, &AccountsDetailWidget::requestDeleteAccount, m_accountsWorker, &AccountsWorker::deleteAccount);
    connect(w, &AccountsDetailWidget::requestSetGroups, m_accountsWorker, &AccountsWorker::setGroups);
    connect(w, &AccountsDetailWidget::requestBack, this, [&]() {
        m_accountsWidget->setShowFirstUserInfo(false);
    });
    connect(w, &AccountsDetailWidget::requestSetAvatar, m_accountsWorker, &AccountsWorker::setAvatar);
    connect(w, &AccountsDetailWidget::requestSetFullname, m_accountsWorker, &AccountsWorker::setFullname);
    connect(w, &AccountsDetailWidget::requestAddThumbs, this, &AccountsModule::onShowAddThumb);
    connect(w, &AccountsDetailWidget::requestDeleteFingerItem, m_fingerWorker, &FingerWorker::deleteFingerItem);
    connect(w, &AccountsDetailWidget::requestRenameFingerItem, m_fingerWorker, &FingerWorker::renameFingerItem);
    connect(w, &AccountsDetailWidget::noticeEnrollCompleted, m_fingerWorker, &FingerWorker::refreshUserEnrollList);
    connect(w, &AccountsDetailWidget::requsetSetPassWordAge, m_accountsWorker, &AccountsWorker::setMaxPasswordAge);
    m_frameProxy->pushWidget(this, w);
    w->setVisible(true);
    m_isCreatePage = false;
    //当前页面为用户详情页面的时候允许跳转默认帐户，否则只响应用户点击。避免出现显示了创建账户页面，又被账户详情页面隐藏的问题。
    m_accountsWidget->setShowDefaultAccountInfo(true);
}

//创建账户界面
void AccountsModule::onShowCreateAccountPage()
{
    if (m_isCreatePage) {
        return;
    }
    CreateAccountPage *w = new CreateAccountPage(m_accountsWorker);
    w->setVisible(false);
    User *newUser = new User(this);
    w->setModel(m_userModel, newUser);
    connect(w, &CreateAccountPage::requestCreateUser, m_accountsWorker, &AccountsWorker::createAccount);
    connect(m_accountsWorker, &AccountsWorker::accountCreationFinished, w, &CreateAccountPage::setCreationResult);
    connect(w, &CreateAccountPage::requestBack, m_accountsWidget, &AccountsWidget::handleRequestBack);
    m_frameProxy->pushWidget(this, w);
    w->setVisible(true);
    m_isCreatePage = true;
    m_accountsWidget->setShowDefaultAccountInfo(false);
}

AccountsModule::~AccountsModule()
{
    if (m_userModel)
        m_userModel->deleteLater();
    m_userModel = nullptr;

    if (m_accountsWorker)
        m_accountsWorker->deleteLater();
    m_accountsWorker = nullptr;
}

//修改密码界面
void AccountsModule::onShowPasswordPage(User *account)
{
    ModifyPasswdPage *w = new ModifyPasswdPage(account);
    w->setVisible(false);
    connect(w, &ModifyPasswdPage::requestChangePassword, m_accountsWorker, &AccountsWorker::setPassword);
    connect(w, &ModifyPasswdPage::requestBack, m_accountsWidget, &AccountsWidget::handleRequestBack);
    m_frameProxy->pushWidget(this, w);
    w->setVisible(true);
}

//添加指纹界面
void AccountsModule::onShowAddThumb(const QString &name, const QString &thumb)
{
    AddFingeDialog *dlg = new AddFingeDialog(thumb);
    connect(dlg, &AddFingeDialog::requestEnrollThumb, m_fingerWorker, [=] {
        m_fingerWorker->tryEnroll(name, thumb);
    });
    connect(dlg, &AddFingeDialog::requestStopEnroll, m_fingerWorker, &FingerWorker::stopEnroll);
    connect(dlg, &AddFingeDialog::requesetCloseDlg, dlg, [=](const QString &userName) {
        m_fingerWorker->refreshUserEnrollList(userName);
        onSetMainWindowEnabled(true);
        dlg->deleteLater();
    });

    m_fingerWorker->tryEnroll(name, thumb);
    connect(m_fingerWorker, &FingerWorker::tryEnrollResult, dlg, [=] (FingerWorker::EnrollResult res) {
        // 第一次tryEnroll进入时显示添加指纹对话框
        if (m_pMainWindow->isEnabled()) {
            if (res == FingerWorker::Enroll_Success) {
                onSetMainWindowEnabled(false);
                m_fingerModel->resetProgress();
                dlg->setFingerModel(m_fingerModel);
                dlg->setWindowFlags(Qt::Dialog | Qt::Popup | Qt::WindowStaysOnTopHint);
                dlg->setUsername(name);
                dlg->show();
                dlg->setFocus();
                dlg->activateWindow();
            } else {
                m_fingerWorker->stopEnroll(name);
                dlg->deleteLater();
            }
        } else {
            //　已经在添加指纹对话框中的Enroll处理
            if (res == FingerWorker::Enroll_AuthFailed) {
                onSetMainWindowEnabled(true);
                dlg->deleteLater();
            } else {
                dlg->setInitStatus();
            }
        }
    });
}

void AccountsModule::onHandleVaildChanged(const bool isVaild)
{
    if (isVaild) {
        initFingerData();
    } else {
        for (const auto &user : m_userModel->userList()) {
            disconnect(user, &User::nameChanged, m_fingerWorker, &FingerWorker::refreshUserEnrollList);
        }
    }

    //用户运行程序第一次点击账户初始化时
    //调用过程：　initialize ->　refreshDevice -> GetDefaultDevice -> onGetFprDefaultDevFinished -> 获取指纹设备存在状态,发出vaildChanged信号 -> onHandleVaildChanged
    //onHandleVaildChanged第一次被调用就行了，后面指纹设备状态发生变化时，有DevicesChanged信号来通知指纹页面。
    disconnect(m_fingerModel, &FingerModel::vaildChanged, this, &AccountsModule::onHandleVaildChanged);
}

void AccountsModule::initFingerData()
{
    QString currentUserName = m_accountsWorker->getCurrentUserName();
    m_fingerWorker->refreshUserEnrollList(currentUserName);
}

void AccountsModule::onSetMainWindowEnabled(bool isEnabled)
{
    if (m_pMainWindow)
        m_pMainWindow->setEnabled(isEnabled);
}
