#include "accountsmodule.h"

AccountsModule::AccountsModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),

      m_accountsInter(new Accounts("com.deepin.daemon.Accounts", "/com/deepin/daemon/Accounts", QDBusConnection::systemBus(), this)),

      m_accountsWidget(nullptr),
      m_accountsDetail(nullptr)
{
}

void AccountsModule::initialize()
{
    m_accountsInter->setSync(false);

    connect(m_accountsInter, &Accounts::UserListChanged, this, &AccountsModule::onUserListChanged);

    onUserListChanged(m_accountsInter->userList());
}

void AccountsModule::moduleActive()
{
    m_accountsInter->blockSignals(false);
    m_accountsInter->getAllProperties();
}

void AccountsModule::moduleDeactive()
{
    if (m_accountsWidget)
    {
        m_accountsWidget->deleteLater();
        m_accountsWidget = nullptr;
    }

    m_accountsInter->blockSignals(true);
}

ModuleWidget *AccountsModule::moduleWidget()
{
    if (!m_accountsWidget)
    {
        m_accountsWidget = new AccountsWidget;
        connect(m_accountsWidget, &AccountsWidget::showAccountsDetail, this, &AccountsModule::showAccountsDetail);
    }

    return m_accountsWidget;
}

const QString AccountsModule::name() const
{
    return "Accounts";
}

void AccountsModule::showAccountsDetail()
{
    if (!m_accountsDetail)
    {
        QLabel *lbl = new QLabel;
        lbl->setText("account detail widget");
        lbl->setStyleSheet("background-color:gray;");
        m_accountsDetail = new ContentWidget;
        m_accountsDetail->setTitle("account detail");
        m_accountsDetail->setContent(lbl);
    }

    m_frameProxy->pushWidget(this, m_accountsDetail);
}

void AccountsModule::contentPopped(ContentWidget * const w)
{
    if (w == m_accountsDetail)
        m_accountsDetail = nullptr;

    w->deleteLater();
}

void AccountsModule::onUserListChanged(const QStringList &users)
{
    if (m_accountsWidget)
        m_accountsWidget->onUserListChanged(users);
}
