#include "userexpandcontent.h"

UserExpandContent::UserExpandContent(const QString &userPath, QWidget *parent)
    : QWidget(parent),m_userPath(userPath)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setAlignment(Qt::AlignHCenter);
    m_mainLayout->setSpacing(0);

    //get dbus data
    m_accountUser = new DBusAccountUser(userPath, this);

    if (m_accountUser->isValid()){
        initSegmentedControl();
        initAvatarPanel();
        initAutoLogin();
        initUserEnable();
        initAccountType();
        initPassword();

        m_mainLayout->addStretch(1);

        onAccountEnableChanged(!m_accountUser->locked());
    }
}

void UserExpandContent::setIsCurrentUser(bool isCurrentUser)
{
    if (isCurrentUser){
        m_typeLine->setFixedHeight(0);
        m_lockLine->setFixedHeight(0);
        updateSize();
    }
}

void UserExpandContent::onRequestPreDestroy()
{
    m_passwordFrame->preDestroy();
}

void UserExpandContent::initSegmentedControl()
{
    m_segmentedControl = new DSegmentedControl(this);
    m_segmentedControl->addSegmented(tr("Recently Used"));
    m_segmentedControl->addSegmented(tr("Avatar"));
    m_segmentedControl->addSegmented(tr("Webcam"));
    m_segmentedControl->setCurrentIndex(1);

    m_segmentedFrame = new QFrame;
    QVBoxLayout *frameLayout = new QVBoxLayout(m_segmentedFrame);
    frameLayout->setContentsMargins(0, LAYOUT_SPACING, 0, LAYOUT_SPACING);
    frameLayout->setAlignment(Qt::AlignCenter);
    frameLayout->addWidget(m_segmentedControl);
    m_mainLayout->addWidget(m_segmentedFrame, 0, Qt::AlignHCenter);
}

void UserExpandContent::initAvatarPanel()
{
    m_historyAvatarGrid = new AvatarGrid("",this);
    m_allAvatarGrid = new AvatarGrid(m_userPath, this);
    m_cameraPanel = new WebcamAvatarPanel(this);
    connect(m_historyAvatarGrid, &AvatarGrid::avatarSelected, this, &UserExpandContent::onAvatarSelected);
    connect(m_allAvatarGrid, &AvatarGrid::avatarSelected, this, &UserExpandContent::onAvatarSelected);
    connect(m_cameraPanel, &WebcamAvatarPanel::selectedAvatar, this, &UserExpandContent::onAvatarSelected);
    connect(this, &UserExpandContent::changeToSetting, [=](bool value){
        if (!value)
            m_cameraPanel->turnOffCamera();
    });

    m_stackWidget = new QStackedWidget(this);
    connect(m_segmentedControl, &DSegmentedControl::currentChanged, m_stackWidget, &QStackedWidget::setCurrentIndex);
    connect(m_stackWidget, &QStackedWidget::currentChanged, [=](int index){
        m_historyAvatarGrid->setAvatars(m_accountUser->historyIcons());
        m_allAvatarGrid->setAvatars(m_accountUser->iconList() << ADD_AVATAR_ICON);

        updatemAvatarGridSize(index);

        if (m_autoLoginLine)    //after initialization
            updateSize();
    });
    connect(m_accountUser, &DBusAccountUser::IconListChanged, [=]{
        m_historyAvatarGrid->setAvatars(m_accountUser->historyIcons());
        m_allAvatarGrid->setAvatars(m_accountUser->iconList() << ADD_AVATAR_ICON);
        updatemAvatarGridSize(m_stackWidget->currentIndex());
        updateSize(true);
    });

    m_stackWidget->addWidget(m_historyAvatarGrid);
    m_stackWidget->addWidget(m_allAvatarGrid);
    m_stackWidget->addWidget(m_cameraPanel);
    m_stackWidget->setCurrentIndex(1);

    m_mainLayout->addWidget(m_stackWidget, 0, Qt::AlignHCenter);
    m_mainLayout->addWidget(new DSeparatorHorizontal);
}

void UserExpandContent::initAutoLogin()
{
    m_autoLoginLine = new AccountSwitchLine();
    m_autoLoginLine->setTitle("Auto Login");
    m_autoLoginLine->setCheck(m_accountUser->automaticLogin());
    connect(m_autoLoginLine, &AccountSwitchLine::checkChanged, [=](bool s){
        if (m_accountUser->automaticLogin() != s)//just record the valid value(configure file just has single line record,set invalid value will cause error)
            m_accountUser->SetAutomaticLogin(s);
    });
    connect(m_accountUser, &DBusAccountUser::AutomaticLoginChanged, [=]{
        m_autoLoginLine->setCheck(m_accountUser->automaticLogin());
    });

    m_mainLayout->addWidget(m_autoLoginLine);
}

void UserExpandContent::initUserEnable()
{
    m_lockLine = new AccountSwitchLine();
    m_lockLine->setTitle("Enable");
    m_lockLine->setCheck(!m_accountUser->locked());
    connect(m_lockLine, &AccountSwitchLine::checkChanged, [=](bool s){
        m_accountUser->SetLocked(!s);
    });
    connect(m_accountUser, &DBusAccountUser::LockedChanged, [=]{
        m_lockLine->setCheck(!m_accountUser->locked());

        onAccountEnableChanged(!m_accountUser->locked());
    });

    m_mainLayout->addWidget(m_lockLine);
}

void UserExpandContent::initAccountType()
{
    m_typeLine = new AccountTypeLine();
    m_typeLine->setTitle("Account Type");
    m_typeLine->setType(m_accountUser->accountType());
    connect(m_typeLine, &AccountTypeLine::typeChanged, m_accountUser, &DBusAccountUser::SetAccountType);
    connect(m_accountUser, &DBusAccountUser::AccountTypeChanged, [=]{
        m_typeLine->setType(m_accountUser->accountType());
    });

    m_mainLayout->addWidget(m_typeLine);
}

void UserExpandContent::initPassword()
{
    m_passwordFrame = new PasswordFrame();
    connect(m_passwordFrame, &PasswordFrame::sizeChanged, [=]{
        updateSize();
    });
    connect(m_passwordFrame, &PasswordFrame::changePassword, [=](QString password){
        m_accountUser->SetPassword(password);
    });

    m_mainLayout->addWidget(m_passwordFrame);
}

void UserExpandContent::onAvatarSelected(const QString &avatar)
{
    if (avatar == ADD_AVATAR_ICON){
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("Choose a new picture for your Avatar"), QDir::homePath(), tr("Image files (*.jpg *.png *.jpeg)"));
        if (!fileName.isEmpty())
            m_accountUser->SetIconFile(fileName);
    }
    else
        m_accountUser->SetIconFile(avatar);
}

void UserExpandContent::onAccountEnableChanged(bool enabled)
{
    if (enabled) {
        m_segmentedFrame->setFixedHeight(DUI::BUTTON_HEIGHT + LAYOUT_SPACING * 2);
        updatemAvatarGridSize(m_stackWidget->currentIndex());
        m_autoLoginLine->setFixedHeight(DUI::CONTENT_HEADER_HEIGHT);
        m_typeLine->setFixedHeight(DUI::CONTENT_HEADER_HEIGHT);
        m_passwordFrame->setFixedHeight(DUI::CONTENT_HEADER_HEIGHT);
    }
    else {
        m_segmentedFrame->setFixedHeight(0);
        m_stackWidget->setFixedHeight(0);
        m_autoLoginLine->setFixedHeight(0);
        m_typeLine->setFixedHeight(0);
        m_passwordFrame->setFixedHeight(0);
    }

    updateSize(true);
}

void UserExpandContent::updateSize(bool note)
{
    int totalHeight = 0;
    totalHeight += m_stackWidget->height();
    totalHeight += m_segmentedFrame->height();
    totalHeight += m_passwordFrame->height();
    totalHeight += m_typeLine->height();
    totalHeight += m_autoLoginLine->height();
    totalHeight += m_lockLine->height();

    setFixedHeight(totalHeight);

    if (note)
        emit sizeChanged();
}

void UserExpandContent::updatemAvatarGridSize(int stackIndex)
{
    QSize ns;
    switch (stackIndex) {
    case 0:
        m_cameraPanel->turnOffCamera();
        ns = m_historyAvatarGrid->size();
        break;
    case 1:
        m_cameraPanel->turnOffCamera();
        ns = m_allAvatarGrid->size();
        break;
    default:
        m_cameraPanel->turnOnCamera();
        ns = m_cameraPanel->size();
        break;
    }

    m_stackWidget->setFixedSize(ns);
}


