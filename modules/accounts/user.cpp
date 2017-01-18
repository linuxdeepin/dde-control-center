#include "user.h"

using namespace dcc::accounts;

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
    if (name != m_name)
    {
        m_name = name;

        emit nameChanged(m_name);
    }
}

void User::setAutoLogin(const bool autoLogin)
{
    m_autoLogin = autoLogin;

    emit autoLoginChanged(m_autoLogin);
}

void User::setAvatars(const QList<QString> &avatars)
{
    m_avatars = avatars;

    emit avatarListChanged(m_avatars);
}

void User::setCurrentAvatar(const QString &avatar)
{
    if (m_currentAvatar != avatar)
    {
        m_currentAvatar = avatar;

        emit currentAvatarChanged(m_currentAvatar);
    }
}

void User::setPassword(const QString &password)
{
    m_password = password;
}

void User::setRepeatPassword(const QString &repeatPassword)
{
    m_repeatPassword = repeatPassword;
}

void User::setOnline(bool online)
{
    m_online = online;

    emit onlineChanged(online);
}
