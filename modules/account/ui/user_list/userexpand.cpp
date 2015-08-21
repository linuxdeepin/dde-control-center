#include "userexpand.h"

UserExpand::UserExpand(const QString &userPath, QWidget *parent)
    : DBaseExpand(parent),m_userPath(userPath)
{
    m_header = new UserExpandHeader(userPath, this);
    setHeader(m_header);
    setHeaderHeight(HEADER_HEIGHT);
    connect(m_header, &UserExpandHeader::mousePress, [=]{
        setExpand(!expand());
    });
    connect(m_header, &UserExpandHeader::cancelDelete, this, &UserExpand::cancelDelete);
    connect(this, &UserExpand::requestDelete, m_header, &UserExpandHeader::changeToDeleteState);

    m_content = new UserExpandContent(userPath, this);
    connect(m_content, &UserExpandContent::sizeChanged, this, &UserExpand::updateContentHeight);

    setContent(m_content);
}

void UserExpand::setIsCurrentUser(bool v)
{
    m_header->setIsCurrentUser(v);
    setHeaderHeight(v ? HEADER_CURRENT_USER_HEIGHT : HEADER_HEIGHT);
}

void UserExpand::setExpand(bool v)
{
    m_header->setExpand(v);
    DBaseExpand::setExpand(v);
}
QString UserExpand::userPath() const
{
    return m_userPath;
}


