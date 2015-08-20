#include "userheader.h"

UserHeader::UserHeader(const QString &userPath, QWidget *parent)
    : QLabel(parent), m_userPath(userPath)
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
    if (m_accountUser->isValid())
        initData();
}

void UserHeader::updateIcon()
{
    m_icon->setIcon(m_accountUser->iconFile());
}

void UserHeader::updateAccountName()
{
    m_nameTitle->setUserName(m_accountUser->userName());
}

void UserHeader::updateAccountType()
{
    m_nameTitle->setUserType(getTypeName(m_accountUser->accountType()));
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

void UserHeader::mousePressEvent(QMouseEvent *)
{
    reverseArrowDirection();
    emit mousePress();
}

void UserHeader::initData()
{
    updateIcon();
    updateAccountName();
    updateAccountType();

    connect(m_accountUser, &DBusAccountUser::IconFileChanged, this, &UserHeader::updateIcon);
    connect(m_accountUser, &DBusAccountUser::AccountTypeChanged, this, &UserHeader::updateAccountType);
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




