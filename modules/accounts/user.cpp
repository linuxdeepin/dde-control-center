#include "user.h"

User::User(QObject *parent)
    : QObject(parent)
{
}

const QString User::name() const
{
    return m_name;
}

void User::setName(const QString &name)
{
    m_name = name;

    emit nameChanged(m_name);
}

void User::setAutoLogin(const bool autoLogin)
{
    m_autoLogin = autoLogin;

    emit autoLoginChanged(m_autoLogin);
}
