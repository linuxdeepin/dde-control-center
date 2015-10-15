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
    connect(this, &UserExpand::changeToSetting, m_content, &UserExpandContent::changeToSetting);
    connect(this, &UserExpand::requestPreDestroy, m_content, &UserExpandContent::onRequestPreDestroy);

    setContent(m_content);

    m_animation = new QPropertyAnimation(this, "height", this);
    m_animation->setDuration(ANIMATION_DURATION);
    m_animation->setEasingCurve(ANIMATION_CURVE);
    connect(m_animation, &QPropertyAnimation::finished, [=]{
        //for expandable
        if ((!m_isCurrentUser && height() == HEADER_HEIGHT)
                || (m_isCurrentUser && height() == HEADER_CURRENT_USER_HEIGHT)){
            setMaximumHeight(10000);
            setMinimumHeight(0);
        }
    });
}

void UserExpand::setIsCurrentUser(bool v)
{
    m_isCurrentUser = v;
    setHeaderHeight(v ? HEADER_CURRENT_USER_HEIGHT : HEADER_HEIGHT);
}

void UserExpand::setExpand(bool v)
{
    if (v){
        m_inSetting = true;
        emit changeToSetting(true);
    }
    else{
        m_inSetting = false;
        emit changeToSetting(false);
    }
    m_header->setExpand(v);
    DBaseExpand::setExpand(v);
}

void UserExpand::showNormal()
{
    if (expand()){
        m_header->setExpand(false);
        DBaseExpand::setExpand(false);
        return;
    }
    int endValue = m_isCurrentUser ? HEADER_CURRENT_USER_HEIGHT : HEADER_HEIGHT;

    m_animation->setStartValue(0);
    m_animation->setEndValue(endValue);
    m_animation->stop();
    m_animation->start();
}

void UserExpand::hide()
{
    if (m_inSetting || height() == 0){
        m_inSetting = false;
        return;
    }

    m_animation->setStartValue(m_isCurrentUser ? HEADER_CURRENT_USER_HEIGHT : HEADER_HEIGHT);
    m_animation->setEndValue(0);
    m_animation->stop();
    m_animation->start();
}
QString UserExpand::userPath() const
{
    return m_userPath;
}

void UserExpand::setHeight(int height)
{
    setFixedHeight(height);
}


