#include "accountsmodule.h"
#include "accountsdetailwidget.h"
#include "modifypasswordpage.h"
#include "modifyavatarpage.h"

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

    m_userList->moveToThread(qApp->thread());
    m_accountsWorker->moveToThread(qApp->thread());

    connect(m_accountsWorker, &AccountsWorker::requestFrameAutoHide, this, &AccountsModule::setFrameAutoHide);
}

void AccountsModule::moduleActive()
{
}

void AccountsModule::moduleDeactive()
{
}

void AccountsModule::reset()
{

}

ModuleWidget *AccountsModule::moduleWidget()
{
    if (!m_accountsWidget)
    {
        m_accountsWidget = new AccountsWidget;

        connect(m_accountsWidget, &AccountsWidget::showAccountsDetail, this, &AccountsModule::showAccountsDetail);
        connect(m_userList, &UserModel::userAdded, m_accountsWidget, &AccountsWidget::addUser);
        connect(m_userList, &UserModel::userRemoved, m_accountsWidget, &AccountsWidget::removeUser);

        for (auto user : m_userList->userList())
            m_accountsWidget->addUser(user);
    }

    return m_accountsWidget;
}

const QString AccountsModule::name() const
{
    return "Accounts";
}

AccountsModule::~AccountsModule()
{
    m_userList->deleteLater();
    m_accountsWorker->deleteLater();

    if (m_accountsWidget)
        m_accountsWidget->deleteLater();
}

void AccountsModule::showAvatarPage(User *account)
{
    ModifyAvatarPage *w = new ModifyAvatarPage(account);

    m_frameProxy->pushWidget(this, w);
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

    connect(w, &AccountsDetailWidget::requestSetAutoLogin, m_accountsWorker, &AccountsWorker::setAutoLogin);
    connect(w, &AccountsDetailWidget::requestCreateAccount, m_accountsWorker, &AccountsWorker::createAccount);
    connect(w, &AccountsDetailWidget::requestDeleteAccount, m_accountsWorker, &AccountsWorker::deleteAccount);
    connect(w, &AccountsDetailWidget::showPwdSettings, this, &AccountsModule::showPasswordPage);
    connect(w, &AccountsDetailWidget::showAvatarSettings, this, &AccountsModule::showAvatarPage);

    m_frameProxy->pushWidget(this, w);
}

void AccountsModule::contentPopped(ContentWidget * const w)
{
    w->deleteLater();
}

void AccountsModule::setFrameAutoHide(const bool autoHide)
{
    m_frameProxy->setFrameAutoHide(this, autoHide);
}
