#include "accountsmodule.h"
#include "accountsdetailwidget.h"
#include "modifypasswordpage.h"
#include "modifyavatarpage.h"
#include "createpage.h"

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

    m_accountsWorker->active();

    connect(m_accountsWorker, &AccountsWorker::requestFrameAutoHide, this, &AccountsModule::setFrameAutoHide);
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

        connect(m_accountsWidget, &AccountsWidget::showAccountsDetail, this, &AccountsModule::showAccountsDetail);
        connect(m_userList, &UserModel::userAdded, m_accountsWidget, &AccountsWidget::addUser);
        connect(m_userList, &UserModel::userRemoved, m_accountsWidget, &AccountsWidget::removeUser);
        connect(m_accountsWidget, &AccountsWidget::requestCreateAccount, this, &AccountsModule::showCreateAccountPage);

        for (auto user : m_userList->userList())
            m_accountsWidget->addUser(user);
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
    connect(w, &AccountsDetailWidget::requestDeleteAccount, m_accountsWorker, &AccountsWorker::deleteAccount);
    connect(w, &AccountsDetailWidget::showPwdSettings, this, &AccountsModule::showPasswordPage);
    connect(w, &AccountsDetailWidget::showAvatarSettings, this, &AccountsModule::showAvatarPage);

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

void AccountsModule::contentPopped(ContentWidget * const w)
{
    w->deleteLater();
}

void AccountsModule::setFrameAutoHide(const bool autoHide)
{
    m_frameProxy->setFrameAutoHide(this, autoHide);
}
