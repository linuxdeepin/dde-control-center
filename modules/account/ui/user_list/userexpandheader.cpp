#include "userexpandheader.h"

UserExpandHeader::UserExpandHeader(const QString &userPath, QWidget *parent)
    : QLabel(parent), m_userPath(userPath)
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignVCenter);

    initIcon();
    initRightStack();

    //get dbus data
    m_accountUser = new DBusAccountUser(userPath, this);
    if (m_accountUser->isValid())
        initData();
}

void UserExpandHeader::updateIcon()
{
    m_icon->setIcon(m_accountUser->iconFile());
}

void UserExpandHeader::updateAccountName()
{
    m_nameTitle->setUserName(m_accountUser->userName());
}

void UserExpandHeader::updateAccountType()
{
    m_nameTitle->setUserType(getTypeName(m_accountUser->accountType()));
}

void UserExpandHeader::onCancelDeleteUser()
{

}

void UserExpandHeader::onConfirmDeleteUser()
{

}

void UserExpandHeader::setIsCurrentUser(bool isCurrentUser)
{
    m_nameTitle->setIsCurrentUser(isCurrentUser);
    m_icon->setIsNormal(!isCurrentUser);
}

void UserExpandHeader::setExpand(bool value)
{
    if (value)
        m_arrowButton->setArrowDirection(DArrowButton::ArrowUp);
    else
        m_arrowButton->setArrowDirection(DArrowButton::ArrowDown);
}

void UserExpandHeader::mousePressEvent(QMouseEvent *)
{
    reverseArrowDirection();
    emit mousePress();
}

void UserExpandHeader::initData()
{
    updateIcon();
    updateAccountName();
    updateAccountType();

    connect(m_accountUser, &DBusAccountUser::IconFileChanged, this, &UserExpandHeader::updateIcon);
    connect(m_accountUser, &DBusAccountUser::AccountTypeChanged, this, &UserExpandHeader::updateAccountType);
}

void UserExpandHeader::initIcon()
{
    m_icon = new UserIcon();
    m_icon->setFixedSize(ICON_WIDTH, ICON_NORMAL_HEIGHT);

    m_mainLayout->addWidget(m_icon);
}

void UserExpandHeader::initRightStack()
{
    m_nameTitle = new UserNameTitle(this);
    m_arrowButton = new DArrowButton(this);
    m_arrowButton->setFixedSize(50, 20);
    connect(m_arrowButton, &DArrowButton::mousePress, [=]{
        reverseArrowDirection();
        emit mousePress();
    });
    QFrame *normalFrame = new QFrame;
    QHBoxLayout *normalLayout = new QHBoxLayout(normalFrame);
    normalLayout->setContentsMargins(0, 0, 0, 0);
    normalLayout->addWidget(m_nameTitle);
    normalLayout->addStretch();
    normalLayout->addWidget(m_arrowButton);

    DSegmentedControl *folderControl = new DSegmentedControl;
    folderControl->addSegmented(tr("Keep Folder"));
    folderControl->addSegmented(tr("Delete Folder"));
    ConfirmButtonLine *confirmLine = new ConfirmButtonLine;
    connect(confirmLine, &ConfirmButtonLine::cancel, this, &UserExpandHeader::onCancelDeleteUser);
    connect(confirmLine, &ConfirmButtonLine::confirm, this, &UserExpandHeader::onConfirmDeleteUser);
    QFrame *deleteFrame = new QFrame;
    QVBoxLayout *deleteLayout = new QVBoxLayout(deleteFrame);
    deleteLayout->setAlignment(Qt::AlignCenter);
    deleteLayout->setContentsMargins(0, 0, 20, 0);
    deleteLayout->addWidget(folderControl);
    deleteLayout->addWidget(confirmLine);

    m_rightStack = new QStackedWidget;
    m_rightStack->addWidget(normalFrame);
    m_rightStack->addWidget(deleteFrame);
    m_mainLayout->addWidget(m_rightStack);
}

void UserExpandHeader::reverseArrowDirection()
{
    if (m_arrowButton->arrowDirection() == DArrowButton::ArrowUp)
        m_arrowButton->setArrowDirection(DArrowButton::ArrowDown);
    else
        m_arrowButton->setArrowDirection(DArrowButton::ArrowUp);
}

QString UserExpandHeader::getTypeName(int type)
{
    switch (type) {
    case 1:
        return tr("Administrator");
    case 0:
        return tr("Normal User");
    default:
        return tr("Unknown User Type");
    }
}




