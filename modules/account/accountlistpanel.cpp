#include "accountlistpanel.h"

AccountListPanel::AccountListPanel(QWidget *parent) : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignVCenter | Qt::AlignTop);

    initSessionManager();
    initAccount();
}

void AccountListPanel::initSessionManager()
{
    m_sessionManager = new DBusSessionManager(this);
}

void AccountListPanel::initAccount()
{
    m_account = new DBusAccount(this);
    connect(m_account, &DBusAccount::UserAdded, this, &AccountListPanel::onUserAdded);
    connect(m_account, &DBusAccount::UserDeleted, this, &AccountListPanel::onUserDeleted);

    if (!m_account->isValid())
        return;

    m_expandGroup = new DExpandGroup(this);

    QStringList userList = m_account->userList();

    //add current user first
    QString currentUserPath = m_account->FindUserById(m_sessionManager->currentUid()).value();
    AccountExpand *currentUser = new AccountExpand(currentUserPath, this);
    currentUser->setIsCurrentUser(true);
    m_expandGroup->addExpand(currentUser);
    m_expands.insert(currentUserPath, currentUser);
    m_mainLayout->addWidget(currentUser);

    //add other user
    foreach (QString path, userList) {
        if (path == currentUserPath)
            continue;

        AccountExpand *userExpand = new AccountExpand(path, this);
        m_expandGroup->addExpand(userExpand);
        m_expands.insert(path, userExpand);
        m_mainLayout->addWidget(userExpand);
    }

    m_mainLayout->addStretch();
}

void AccountListPanel::onUserAdded(const QString &path)
{
    AccountExpand *expand = new AccountExpand(path, this);
    m_expandGroup->addExpand(expand);
    m_expands.insert(path, expand);
    m_mainLayout->insertWidget(m_mainLayout->count() - 1, expand);
}

void AccountListPanel::onUserDeleted(const QString &path)
{
    AccountExpand *target = m_expands.value(path);
    m_expandGroup->removeExpand(target);
    m_mainLayout->removeWidget(target);
    target->deleteLater();
}

