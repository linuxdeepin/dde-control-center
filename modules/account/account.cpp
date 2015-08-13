#include <QLabel>
#include <QDebug>

#include "account.h"

Account::Account()
{
    m_label = new QLabel("Account");

    /*
    SessionManager { id: dbus_session_manager }
    Accounts { id: dbus_accounts }
    User { id: dbus_user }
    User { id: current_user; path: dbus_accounts.FindUserById(dbus_session_manager.currentUid)}
*/
}

Account::~Account()
{
    qDebug() << "~Account()";

    m_label->deleteLater();
}

QFrame* Account::getContent()
{
    return m_label;
}
