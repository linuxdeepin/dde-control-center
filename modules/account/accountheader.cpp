#include "accountheader.h"

AccountHeader::AccountHeader(const QString &userPath, QWidget *parent)
    : QWidget(parent), m_userPath(userPath)
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignVCenter);

    m_icon = new AccountIcon();
    m_icon->setFixedSize(ICON_WIDTH, ICON_NORMAL_HEIGHT);

    m_nameTitle = new AccountNameTitle(this);

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

void AccountHeader::setIcon(const QString &iconPath)
{
    m_icon->setIcon(iconPath);
}

void AccountHeader::setAccountName(const QString &name)
{
    m_nameTitle->setUserName(name);
}

void AccountHeader::setAccountType(const QString &type)
{
    m_nameTitle->setUserType(type);
}

void AccountHeader::setIsCurrentUser(bool isCurrentUser)
{
    m_nameTitle->setIsCurrentUser(isCurrentUser);
    m_icon->setIsNormal(!isCurrentUser);
}

void AccountHeader::setExpand(bool value)
{
    if (value)
        m_arrowButton->setArrowDirection(DArrowButton::ArrowUp);
    else
        m_arrowButton->setArrowDirection(DArrowButton::ArrowDown);
}

void AccountHeader::mousePressEvent(QMouseEvent *event)
{
    reverseArrowDirection();
    emit mousePress();
}

void AccountHeader::initData()
{
    if (m_accountUser && m_accountUser->isValid()){
        setIcon(m_accountUser->iconFile());
        setAccountName(m_accountUser->userName());
        setAccountType(getTypeName(m_accountUser->accountType()));
    }
}

void AccountHeader::reverseArrowDirection()
{
    if (m_arrowButton->arrowDirection() == DArrowButton::ArrowUp)
        m_arrowButton->setArrowDirection(DArrowButton::ArrowDown);
    else
        m_arrowButton->setArrowDirection(DArrowButton::ArrowUp);
}

QString AccountHeader::getTypeName(int type)
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




