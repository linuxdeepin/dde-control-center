#include "user.h"

User::User(QObject *parent)
    : QObject(parent)
{
}

QString User::name() const
{
    return m_name;
}

void User::setName(const QString &name)
{
    if (m_name == name) return;

    m_name = name;
    emit nameChanged(m_name);
}
