#include "accountsmodule.h"
#include "accountsdetailwidget.h"

AccountsModule::AccountsModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),
      m_userList(new UserModel(this)),
      m_accountsWidget(new AccountsWidget),
      m_accountsWorker(new AccountsWorker(m_userList, this))
{
    connect(m_accountsWidget, &AccountsWidget::showAccountsDetail, this, &AccountsModule::showAccountsDetail);
}

void AccountsModule::initialize()
{
//    m_accountsInter->setSync(false);

//    connect(m_accountsInter, &Accounts::UserListChanged, this, &AccountsModule::onUserListChanged);

//    onUserListChanged(m_accountsInter->userList());

    connect(m_userList, &UserModel::userAdded, m_accountsWidget, &AccountsWidget::addUser);
    connect(m_userList, &UserModel::userRemoved, m_accountsWidget, &AccountsWidget::removeUser);
}

void AccountsModule::moduleActive()
{
//    m_accountsInter->blockSignals(false);
//    m_accountsInter->getAllProperties();
}

void AccountsModule::moduleDeactive()
{
//    if (m_accountsWidget)
//    {
//        m_accountsWidget->deleteLater();
//        m_accountsWidget = nullptr;
//    }

//    m_accountsInter->blockSignals(true);
}

ModuleWidget *AccountsModule::moduleWidget()
{
    return m_accountsWidget;
}

const QString AccountsModule::name() const
{
    return "Accounts";
}

void AccountsModule::showAccountsDetail(User *account)
{
    AccountsDetailWidget *w = new AccountsDetailWidget(account);

    connect(w, &AccountsDetailWidget::requestSetAutoLogin, m_accountsWorker, &AccountsWorker::setAutoLogin);

    m_frameProxy->pushWidget(this, w);
}

void AccountsModule::contentPopped(ContentWidget * const w)
{
    w->deleteLater();
}
