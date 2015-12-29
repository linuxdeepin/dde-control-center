#include "userlistpanel.h"

UserListPanel::UserListPanel(QWidget *parent) : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignVCenter | Qt::AlignTop);

    initSessionManager();

    //the "initAccount" slot is invoked as soon as the application enters the main event loop.
    //make sure the dbus interface is ready
    QMetaObject::invokeMethod(this, "initAccount", Qt::QueuedConnection);
}

void UserListPanel::preDestroy()
{
    emit requestPreDestroy();
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

    QStringList userList = m_account->userList();

    //add current user first
    QString currentUserPath = m_account->FindUserById(m_sessionManager->currentUid()).value();
    UserExpand *currentUser = new UserExpand(currentUserPath, this);
    currentUser->setIsCurrentUser(true);
    m_expandList.append(currentUser);
    m_mainLayout->addWidget(currentUser);
    m_mainLayout->addStretch(1);
    connect(currentUser, &UserExpand::expandChange, this, &UserListPanel::adjustSize);
    connect(currentUser, &UserExpand::changeToSetting, this, &UserListPanel::changeToSetting);
    connect(this, &UserListPanel::requestPreDestroy, currentUser, &UserExpand::requestPreDestroy);
    connect(this, &UserListPanel::requestDelete, currentUser, &UserExpand::requestDelete);
    connect(this, &UserListPanel::showForNormal, currentUser, &UserExpand::showNormal);
    connect(this, &UserListPanel::hideForSetting, currentUser, &UserExpand::hide);

    //add other user
    foreach (QString path, userList) {
        if (path == currentUserPath)
            continue;

        UserExpand *userExpand = new UserExpand(path, this);
        connect(userExpand, &UserExpand::cancelDelete, this, &UserListPanel::cancelDelete);
        connect(userExpand, &UserExpand::changeToSetting, this, &UserListPanel::changeToSetting);
        connect(userExpand, &UserExpand::userLockChanged, [=] {
            insertToLayout(userExpand);
        });
        connect(this, &UserListPanel::requestPreDestroy, userExpand, &UserExpand::requestPreDestroy);
        connect(this, &UserListPanel::requestDelete, userExpand, &UserExpand::requestDelete);
        connect(this, &UserListPanel::showForNormal, userExpand, &UserExpand::showNormal);
        connect(this, &UserListPanel::hideForSetting, userExpand, &UserExpand::hide);
        insertToLayout(userExpand);
        connect(userExpand, &UserExpand::expandChange, this, &UserListPanel::adjustSize);
    }

}

void UserListPanel::onUserAdded(const QString &path)
{
    UserExpand *expand = new UserExpand(path, this);
    connect(expand, &UserExpand::cancelDelete, this, &UserListPanel::cancelDelete);
    connect(expand, &UserExpand::changeToSetting, this, &UserListPanel::changeToSetting);
    connect(expand, &UserExpand::userLockChanged, [=] {
        insertToLayout(expand);
    });
    connect(this, &UserListPanel::requestPreDestroy, expand, &UserExpand::requestPreDestroy);
    connect(this, &UserListPanel::requestDelete, expand, &UserExpand::requestDelete);
    connect(this, &UserListPanel::showForNormal, expand, &UserExpand::showNormal);
    connect(this, &UserListPanel::hideForSetting, expand, &UserExpand::hide);

    insertToLayout(expand);
}

void UserListPanel::insertToLayout(UserExpand *expand)
{
    //remove from old position
    if (m_expandList.indexOf(expand) != -1) {
        m_expandList.removeAll(expand);
        m_mainLayout->removeWidget(expand);
    }

    if (expand->userLocked()) { //there is a Stretch in the end of the layout
        m_mainLayout->insertWidget(m_mainLayout->count() - 1, expand);
    }
    else {  //there must be a current-user in the list
        m_mainLayout->insertWidget(1, expand);
    }

    m_expandList.append(expand);
}

void UserListPanel::onUserDeleted(const QString &path)
{
    for (UserExpand *expand : m_expandList) {
        if (expand->userPath() == path) {
            m_expandList.removeAll(expand);
            m_mainLayout->removeWidget(expand);
            expand->deleteLater();

            if (m_expandList.count() == 1) {    //no user for delete
                emit cancelDelete();
            }

            break;
        }
    }
}

