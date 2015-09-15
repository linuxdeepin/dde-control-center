#include "userexpandheader.h"

UserExpandHeader::UserExpandHeader(const QString &userPath, QWidget *parent)
    : QLabel(parent), m_userPath(userPath)
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignVCenter);

    initDeleteButton();
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
    changeToDeleteState(false);
}

void UserExpandHeader::onConfirmDeleteUser()
{
    DBusAccount *account = new DBusAccount(this);
    if (account->isValid())
        account->DeleteUser(m_accountUser->userName(), m_folderControl->currentIndex() != 0);
    account->deleteLater();
}

void UserExpandHeader::setIsCurrentUser(bool isCurrentUser)
{
    m_nameTitle->setIsCurrentUser(isCurrentUser);
    UserAvatar::AvatarSize size = isCurrentUser ? UserAvatar::AvatarLargeSize : UserAvatar::AvatarNormalSize;
    m_icon->setAvatarSize(size);
}

void UserExpandHeader::setExpand(bool value)
{
    if (value)
        m_arrowButton->setArrowDirection(DArrowButton::ArrowUp);
    else
        m_arrowButton->setArrowDirection(DArrowButton::ArrowDown);
}

void UserExpandHeader::changeToDeleteState(bool value)
{
    if (value){
        m_arrowButton->setVisible(false);
        m_deleteButton->showIcon();
    }
    else{
        m_arrowButton->setVisible(true);
        m_rightStack->setCurrentIndex(0);
        m_deleteButton->hideIcon();
    }
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
    m_icon = new UserAvatar();
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

    m_folderControl = new DSegmentedControl;
    m_folderControl->addSegmented(tr("Keep Folder"));
    m_folderControl->addSegmented(tr("Delete Folder"));

    DTextButton *cancelButton = new DTextButton(tr("Cancel"));
    DTextButton *confirmButton = new DTextButton(tr("Confirm"));
    connect(cancelButton, &DTextButton::clicked, this, &UserExpandHeader::cancelDelete);
    connect(cancelButton, &DTextButton::clicked, this, &UserExpandHeader::onCancelDeleteUser);
    connect(confirmButton, &DTextButton::clicked, this, &UserExpandHeader::onConfirmDeleteUser);
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setAlignment(Qt::AlignRight);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addSpacing(DUI::BUTTON_MARGIN);
    buttonLayout->addWidget(confirmButton);
    buttonLayout->addSpacing(DUI::HEADER_RIGHT_MARGIN);

    QFrame *deleteFrame = new QFrame;
    QVBoxLayout *deleteLayout = new QVBoxLayout(deleteFrame);
    deleteLayout->setContentsMargins(0, 0, 0, 0);
    deleteLayout->setSpacing(0);
    deleteLayout->addWidget(m_folderControl);
    deleteLayout->addLayout(buttonLayout);

    m_rightStack = new QStackedWidget;
    m_rightStack->addWidget(normalFrame);
    m_rightStack->addWidget(deleteFrame);
    m_mainLayout->addWidget(m_rightStack);
}

void UserExpandHeader::initDeleteButton()
{
    m_deleteButton = new DeleteButton;
    m_deleteButton->hideIcon();
    m_mainLayout->addWidget(m_deleteButton);
    connect(m_deleteButton, &DeleteButton::clicked, [=]{
        m_rightStack->setCurrentIndex(1);
        m_deleteButton->hideIcon();
    });
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


DeleteButton::DeleteButton(QWidget *parent) : QFrame(parent)
{
    m_deleteButton = new MultiDeleteButton;
    connect(m_deleteButton, &MultiDeleteButton::clicked, this, &DeleteButton::clicked);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignVCenter);

    layout->addSpacing(14);
    layout->addWidget(m_deleteButton);
    layout->addSpacing(22);

    m_animation = new QPropertyAnimation(this, "width",this);
    m_animation->setDuration(ANIMATION_DURATION);
    m_animation->setEasingCurve(ANIMATION_CURVE);
}

void DeleteButton::showIcon()
{
    m_animation->setStartValue(0);
    m_animation->setEndValue(ICON_WIDTH);
    m_animation->stop();
    m_animation->start();
}

void DeleteButton::hideIcon()
{
    if (width() == 0)
        return;
    m_animation->setStartValue(ICON_WIDTH);
    m_animation->setEndValue(0);
    m_animation->stop();
    m_animation->start();
}
