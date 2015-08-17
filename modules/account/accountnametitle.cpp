#include "accountnametitle.h"

AccountNameTitle::AccountNameTitle(QWidget *parent) : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_typeLabel = new QLabel(this);
    m_typeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    m_mainLayout->addWidget(m_nameLabel);
    m_mainLayout->addWidget(m_typeLabel);
}

void AccountNameTitle::setUserName(const QString &name)
{
    m_nameLabel->setText(name);
}

void AccountNameTitle::setUserType(const QString &type)
{
    m_typeLabel->setText(type);
}

void AccountNameTitle::setIsCurrentUser(bool v)
{
    m_isCurrentUser = v;
}
