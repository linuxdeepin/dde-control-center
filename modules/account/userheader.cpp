#include "userheader.h"

UserHeader::UserHeader(const QString &userPath, QWidget *parent)
    : QWidget(parent), m_userPath(userPath)
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignVCenter);

    m_icon = new UserIcon();
    m_icon->setFixedSize(ICON_WIDTH, ICON_NORMAL_HEIGHT);

    m_nameTitle = new UserNameTitle(this);

    m_arrowButton = new DArrowButton(this);
    m_arrowButton->setFixedSize(50, 20);
    connect(m_arrowButton, &DArrowButton::mousePress, [=]{
        reverseArrowDirection();
        emit mousePress();
    });

    m_mainLayout->addWidget(m_icon);
    m_mainLayout->addWidget(m_nameTitle);
    m_mainLayout->addStretch();
    m_mainLayout->addWidget(m_arrowButton);

    //get dbus data
    m_accountUser = new DBusAccountUser(userPath, this);
    initData();
}

void UserHeader::setIcon(const QString &iconPath)
{
    m_icon->setIcon(iconPath);
}

void UserHeader::setAccountName(const QString &name)
{
    m_nameTitle->setUserName(name);
}

void UserHeader::setAccountType(const QString &type)
{
    m_nameTitle->setUserType(type);
}

void UserHeader::setIsCurrentUser(bool isCurrentUser)
{
    m_nameTitle->setIsCurrentUser(isCurrentUser);
    m_icon->setIsNormal(!isCurrentUser);
}

void UserHeader::setExpand(bool value)
{
    if (value)
        m_arrowButton->setArrowDirection(DArrowButton::ArrowUp);
    else
        m_arrowButton->setArrowDirection(DArrowButton::ArrowDown);
}

void UserHeader::mousePressEvent(QMouseEvent *event)
{
    reverseArrowDirection();
    emit mousePress();
}

void UserHeader::initData()
{
    if (m_accountUser && m_accountUser->isValid()){
        setIcon(m_accountUser->iconFile());
        setAccountName(m_accountUser->userName());
        setAccountType(getTypeName(m_accountUser->accountType()));
    }
}

void UserHeader::reverseArrowDirection()
{
    if (m_arrowButton->arrowDirection() == DArrowButton::ArrowUp)
        m_arrowButton->setArrowDirection(DArrowButton::ArrowDown);
    else
        m_arrowButton->setArrowDirection(DArrowButton::ArrowUp);
}

QString UserHeader::getTypeName(int type)
{
    switch (type) {
    case 1:
        return "Administrator";
    case 0:
        return "Normal User";
    default:
        return "Unknown User Type";
    }
}




