#include "userlistpanel.h"

UserListPanel::UserListPanel(QWidget *parent) : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignVCenter | Qt::AlignTop);

    initSessionManager();
    initAccount();
}

void UserListPanel::initSessionManager()
{
    m_sessionManager = new DBusSessionManager(this);
}

void UserListPanel::initAccount()
{
    m_account = new DBusAccount(this);
    connect(m_account, &DBusAccount::UserAdded, this, &UserListPanel::onUserAdded);
    connect(m_account, &DBusAccount::UserDeleted, this, &UserListPanel::onUserDeleted);

    if (!m_account->isValid())
        return;

    m_expandGroup = new DExpandGroup(this);

    QStringList userList = m_account->userList();

    //add current user first
    QString currentUserPath = m_account->FindUserById(m_sessionManager->currentUid()).value();
    UserExpand *currentUser = new UserExpand(currentUserPath, this);
    currentUser->setIsCurrentUser(true);
    m_expandGroup->addExpand(currentUser);
    m_expands.insert(currentUserPath, currentUser);
    m_mainLayout->addWidget(currentUser);
    connect(currentUser, &UserExpand::expandChange, this, &UserListPanel::adjustSize);

    //add other user
    foreach (QString path, userList) {
        if (path == currentUserPath)
            continue;

        UserExpand *userExpand = new UserExpand(path, this);
        m_expandGroup->addExpand(userExpand);
        m_expands.insert(path, userExpand);
        m_mainLayout->addWidget(userExpand);
        connect(userExpand, &UserExpand::expandChange, this, &UserListPanel::adjustSize);
    }

    m_mainLayout->addStretch(1);
}

void UserListPanel::onUserAdded(const QString &path)
{
    UserExpand *expand = new UserExpand(path, this);
    m_expandGroup->addExpand(expand);
    m_expands.insert(path, expand);
    m_mainLayout->insertWidget(m_mainLayout->count() - 1, expand);
}

void UserListPanel::onUserDeleted(const QString &path)
{
    UserExpand *target = m_expands.value(path);
    m_expandGroup->removeExpand(target);
    m_mainLayout->removeWidget(target);
    target->deleteLater();
}

