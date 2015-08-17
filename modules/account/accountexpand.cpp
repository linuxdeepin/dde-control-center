#include "accountexpand.h"

AccountExpand::AccountExpand(const QString &userPath, QWidget *parent)
    : DBaseExpand(parent),m_userPath(userPath)
{
    m_header = new AccountHeader(userPath, this);
    setHeader(m_header);
    setHeaderHeight(HEADER_CURRENT_USER_HEIGHT);
    connect(m_header, &AccountHeader::mousePress, [=]{
        setExpand(!expand());
    });

    m_content = new AccountContent(userPath, this);
    m_content->setFixedSize(320, 300);
    setContent(m_content);
}

void AccountExpand::setIsCurrentUser(bool v)
{
    m_header->setIsCurrentUser(v);
    setHeaderHeight(v ? HEADER_CURRENT_USER_HEIGHT : HEADER_HEIGHT);
}

void AccountExpand::setExpand(bool v)
{
    m_header->setExpand(v);
    DBaseExpand::setExpand(v);
}
QString AccountExpand::userPath() const
{
    return m_userPath;
}



