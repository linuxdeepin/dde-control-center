/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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
#include "accountsdetailwidget.h"
#include "modifypasswordpage.h"
#include "modifyavatarpage.h"
#include "modifyfullnamepage.h"
#include "createpage.h"
#include "fingerpage.h"
#include "addfingerpage.h"
#include "fingermodel.h"

#ifdef DCC_ENABLE_ADDOMAIN
#include "widgets/addialog.h"
#endif

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::accounts;

AccountsModule::AccountsModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),

      m_accountsWidget(nullptr)
{
}

void AccountsModule::initialize()
{
    m_userList = new UserModel;
    m_accountsWorker = new AccountsWorker(m_userList);

    m_accountsWorker->moveToThread(qApp->thread());
    m_userList->moveToThread(qApp->thread());

    m_fingerModel = new FingerModel;
    m_fingerWorker = new FingerWorker(m_fingerModel);

    m_fingerModel->moveToThread(qApp->thread());
    m_fingerWorker->moveToThread(qApp->thread());

    m_accountsWorker->active();

    connect(m_accountsWorker, &AccountsWorker::requestFrameAutoHide, this, &AccountsModule::setFrameAutoHide);
    connect(m_fingerWorker, &FingerWorker::requestShowAddThumb, this, &AccountsModule::showAddThumb);
}

void AccountsModule::moduleActive()
{
    m_accountsWorker->active();
}

void AccountsModule::moduleDeactive()
{
}

void AccountsModule::reset()
{

}

void AccountsModule::showPage(const QString &pageName)
{
    Q_ASSERT(m_accountsWidget);

    for (auto *u : m_userList->userList())
        if (u->name() == pageName)
            return showAccountsDetail(u);

    qWarning() << "user not found" << pageName;
}

ModuleWidget *AccountsModule::moduleWidget()
{
    if (!m_accountsWidget)
    {
        m_accountsWidget = new AccountsWidget;

#ifdef DCC_ENABLE_ADDOMAIN
        m_accountsWidget->setModel(m_userList);
#endif

        connect(m_accountsWidget, &AccountsWidget::showAccountsDetail, this, &AccountsModule::showAccountsDetail);
        connect(m_userList, &UserModel::userAdded, m_accountsWidget, &AccountsWidget::addUser);
        connect(m_accountsWidget, &AccountsWidget::requestCreateAccount, this, &AccountsModule::showCreateAccountPage);
#ifdef DCC_ENABLE_ADDOMAIN
        connect(m_userList, &UserModel::isJoinADDomainChanged, m_accountsWidget, &AccountsWidget::setADState);
        connect(m_accountsWidget, &AccountsWidget::requestShowADDialog, this, &AccountsModule::showADDialog);
        connect(m_accountsWidget, &AccountsWidget::requestRefreshADState, m_accountsWorker, &AccountsWorker::refreshADDomain);
#endif

        for (auto user : m_userList->userList())
            m_accountsWidget->addUser(user);

#ifdef DCC_ENABLE_ADDOMAIN
        m_accountsWidget->setADState(m_userList->isJoinADDomain());
#endif
    }

    return m_accountsWidget;
}

const QString AccountsModule::name() const
{
    return QStringLiteral("accounts");
}

AccountsModule::~AccountsModule()
{
    m_userList->deleteLater();
    m_accountsWorker->deleteLater();
}

void AccountsModule::showAvatarPage(User *account)
{
    ModifyAvatarPage *w = new ModifyAvatarPage(account);

    connect(w, &ModifyAvatarPage::requestAddNewAvatar, m_accountsWorker, &AccountsWorker::addNewAvatar);
    connect(w, &ModifyAvatarPage::requestSetAvatar, m_accountsWorker, &AccountsWorker::setAvatar);
    connect(w, &ModifyAvatarPage::requestDeleteAvatar, m_accountsWorker, &AccountsWorker::deleteUserIcon);

    m_frameProxy->pushWidget(this, w);
}

void AccountsModule::showFullnamePage(User *account)
{
    ModifyFullNamePage *p = new ModifyFullNamePage(account);

    connect(p, &ModifyFullNamePage::requestSetFullname, m_accountsWorker, &AccountsWorker::setFullname);
    connect(m_accountsWorker, &AccountsWorker::accountFullNameChangeFinished, p, &ModifyFullNamePage::back);

    m_frameProxy->pushWidget(this, p);
}

void AccountsModule::showPasswordPage(User *account)
{
    ModifyPasswordPage *w = new ModifyPasswordPage(account);

    connect(w, &ModifyPasswordPage::requestChangePassword, m_accountsWorker, &AccountsWorker::setPassword);

    m_frameProxy->pushWidget(this, w);
}

void AccountsModule::showAccountsDetail(User *account)
{
    AccountsDetailWidget *w = new AccountsDetailWidget(account);
    m_fingerWorker->refreshDevice();
    w->setFingerModel(m_fingerModel);

    connect(w, &AccountsDetailWidget::requestSetAutoLogin, m_accountsWorker, &AccountsWorker::setAutoLogin);
    connect(w, &AccountsDetailWidget::requestDeleteAccount, m_accountsWorker, &AccountsWorker::deleteAccount);
    connect(w, &AccountsDetailWidget::showPwdSettings, this, &AccountsModule::showPasswordPage);
    connect(w, &AccountsDetailWidget::showAvatarSettings, this, &AccountsModule::showAvatarPage);
    connect(w, &AccountsDetailWidget::showFullnameSettings, this, &AccountsModule::showFullnamePage);
    connect(w, &AccountsDetailWidget::showFingerSettings, this, &AccountsModule::showFingerPage);
    connect(w, &AccountsDetailWidget::requestNopasswdLogin, m_accountsWorker, &AccountsWorker::setNopasswdLogin);

    connect(w, &AccountsDetailWidget::requestChangeFrameAutoHide, this, [this] (const bool autoHide) {
        m_frameProxy->setFrameAutoHide(this, autoHide);
    }, Qt::DirectConnection);

    m_frameProxy->pushWidget(this, w);
}

void AccountsModule::showCreateAccountPage()
{
    dcc::accounts::CreatePage *createPage = new dcc::accounts::CreatePage;

    User *newUser = new User(this);
    createPage->setModel(newUser);

    connect(createPage, &dcc::accounts::CreatePage::requestCreateUser, m_accountsWorker, &AccountsWorker::createAccount);
    connect(m_accountsWorker, &AccountsWorker::accountCreationFinished, createPage, &dcc::accounts::CreatePage::setCreationResult);

    m_accountsWorker->randomUserIcon(newUser);
    m_frameProxy->pushWidget(this, createPage);
}

void AccountsModule::showFingerPage(User *account)
{
    m_fingerWorker->refreshUserEnrollList(account->name());

    dcc::accounts::FingerPage *page = new FingerPage(account);
    page->setFingerModel(m_fingerModel);

    connect(page, &FingerPage::requestAddThumbs, m_fingerWorker, &FingerWorker::enrollStart);
    connect(page, &FingerPage::requestCleanThumbs, m_fingerWorker, &FingerWorker::cleanEnroll);
    connect(page, &FingerPage::back, m_fingerWorker, &FingerWorker::stopEnroll);

    m_frameProxy->pushWidget(this, page);
}

void AccountsModule::showAddThumb(const QString &name, const QString &thumb)
{
    AddFingerPage *page = new AddFingerPage(thumb);
    page->setFingerModel(m_fingerModel);
    page->setUsername(name);

    connect(page, &AddFingerPage::requestSaveThumb, m_fingerWorker, &FingerWorker::saveEnroll);
    connect(page, &AddFingerPage::requestReEnrollStart, m_fingerWorker, &FingerWorker::reEnrollStart);
    connect(page, &AddFingerPage::back, m_fingerWorker, &FingerWorker::stopEnroll);

    m_frameProxy->pushWidget(this, page);
}

void AccountsModule::contentPopped(ContentWidget * const w)
{
    w->deleteLater();
}

#ifdef DCC_ENABLE_ADDOMAIN
void AccountsModule::showADDialog()
{
    if (m_userList->isJoinADDomain()) {
        m_accountsWorker->ADDomainHandle("", "", "");
        return;
    }

    setFrameAutoHide(false);

    ADDialog addialog;
    addialog.setUserModel(m_userList);

    connect(&addialog, &ADDialog::requestInfos, m_accountsWorker, &AccountsWorker::ADDomainHandle);

    addialog.exec();

    setFrameAutoHide(true);
}
#endif

void AccountsModule::setFrameAutoHide(const bool autoHide)
{
    m_frameProxy->setFrameAutoHide(this, autoHide);
}
