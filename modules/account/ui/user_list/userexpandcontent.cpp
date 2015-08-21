#include "userexpandcontent.h"

UserExpandContent::UserExpandContent(const QString &userPath, QWidget *parent)
    : QWidget(parent),m_userPath(userPath)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignHCenter);

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

        updateSize(false);
    }
}

void UserExpandContent::initSegmentedControl()
{
    m_segmentedControl = new DSegmentedControl(this);
    m_segmentedControl->addSegmented("Recently Used");
    m_segmentedControl->addSegmented("Avatar");
    m_segmentedControl->addSegmented("Webcam");
//    m_segmentedControl->setMaximumWidth(230);
    m_segmentedControl->setCurrentIndex(1);

    m_mainLayout->addSpacing(LAYOUT_SPACING);
    m_mainLayout->addWidget(m_segmentedControl);
}

void UserExpandContent::initAvatarPanel()
{
    AvatarGrid *historyAvatarGrid = new AvatarGrid(this);
    AvatarGrid *allAvatarGrid = new AvatarGrid(this);
    WebcamAvatarPanel *wap = new WebcamAvatarPanel(this);
    connect(historyAvatarGrid, &AvatarGrid::avatarSelected, this, &UserExpandContent::onAvatarSelected);
    connect(allAvatarGrid, &AvatarGrid::avatarSelected, this, &UserExpandContent::onAvatarSelected);

    m_stackWidget = new QStackedWidget(this);
    connect(m_segmentedControl, &DSegmentedControl::currentChanged, m_stackWidget, &QStackedWidget::setCurrentIndex);
    connect(m_stackWidget, &QStackedWidget::currentChanged, [=](int index){
        historyAvatarGrid->setAvatars(m_accountUser->historyIcons());
        allAvatarGrid->setAvatars(m_accountUser->iconList() << ":/images/images/avatar_add.png");

        QSize ns;
        if (index == 0)
            ns = historyAvatarGrid->size();
        else if (index == 1)
            ns = allAvatarGrid->size();
        else
            ns = wap->size();

        m_stackWidget->setFixedSize(ns);

        if (m_autoLoginLine)    //after initialization
            updateSize();
    });

    m_stackWidget->addWidget(historyAvatarGrid);
    m_stackWidget->addWidget(allAvatarGrid);
    m_stackWidget->addWidget(wap);
    m_stackWidget->setCurrentIndex(1);

    m_mainLayout->addSpacing(LAYOUT_SPACING);
    m_mainLayout->addWidget(m_stackWidget);
}

void UserExpandContent::initAutoLogin()
{
    m_autoLoginLine = new SwitchLine();
    m_autoLoginLine->setTitle("Auto Login");
    m_autoLoginLine->setCheck(m_accountUser->automaticLogin());
    connect(m_autoLoginLine, &SwitchLine::checkChanged, [=](bool s){
        m_accountUser->SetAutomaticLogin(s);
    });
    connect(m_accountUser, &DBusAccountUser::AutomaticLoginChanged, [=]{
        m_autoLoginLine->setCheck(m_accountUser->automaticLogin());
    });

    m_mainLayout->addWidget(m_autoLoginLine);
}

void UserExpandContent::initUserEnable()
{
    m_lockLine = new SwitchLine();
    m_lockLine->setTitle("Enable");
    m_lockLine->setCheck(!m_accountUser->locked());
    connect(m_lockLine, &SwitchLine::checkChanged, [=](bool s){
        m_accountUser->SetLocked(!s);
    });
    connect(m_accountUser, &DBusAccountUser::LockedChanged, [=]{
        m_lockLine->setCheck(!m_accountUser->locked());
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
    if (avatar == ":/images/images/avatar_add.png"){
        //TODO
        //add file from file system
    }
    else
        m_accountUser->SetIconFile(avatar);
}

void UserExpandContent::updateSize(bool note)
{
    int totalHeight = LAYOUT_SPACING * 2;
    totalHeight += m_stackWidget->height();
    totalHeight += m_segmentedControl->height();
    totalHeight += m_passwordFrame->height();
    totalHeight += m_typeLine->height();
    totalHeight += m_autoLoginLine->height();
    totalHeight += m_lockLine->height();

    setFixedHeight(totalHeight);

    if (note)
        emit sizeChanged();
}


