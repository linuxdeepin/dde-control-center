#include <QLabel>
#include <QDebug>

#include "account.h"

Account::Account()
{
    m_label = new QLabel("Account");
    m_mainWidget = new AccountMainWidget;
}

Account::~Account()
{
    qDebug() << "~Account()";

    m_label->deleteLater();
    m_mainWidget->deleteLater();
}

QFrame* Account::getContent()
{
    return m_mainWidget;
}
