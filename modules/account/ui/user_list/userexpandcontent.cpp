#include <QTimer>
#include <QApplication>
#include "dbus/dbusaccount.h"
#include "dbus/dbussessionmanager.h"
#include "userexpandcontent.h"
#include <sys/types.h>
#include <unistd.h>

UserExpandContent::UserExpandContent(const QString &userPath, QWidget *parent)
    : QWidget(parent),m_userPath(userPath)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setAlignment(Qt::AlignHCenter);
    m_mainLayout->setSpacing(0);

    QTimer *initTimer = new QTimer(this);
    connect(initTimer, SIGNAL(timeout()), this, SLOT(initDBusData()));
    initTimer->start(1000);
}

UserExpandContent::~UserExpandContent()
{
    onCanHideControlCenter();
}

void UserExpandContent::onRequestPreDestroy()
{
    if (m_passwordFrame)
        m_passwordFrame->preDestroy();
}

void UserExpandContent::initDBusData()
{
    QTimer *timer = qobject_cast<QTimer *>(sender());
    if (timer) {
        timer->stop();
        timer->deleteLater();

        //get dbus data
        DBusAccount * da = new DBusAccount(this);
        connect(da, &DBusAccount::Success, [=](uint id, const QString &) {
            if (uint(getpid()) == id) {
                //500毫秒延时是为了防止某些性能比较好的机器上dbus数据处理比较快，
                //引起的返回比较快导致窗口焦点切换还没完成就设置控制中心为“可隐藏”状态
                //这是比较折中的处理方案
                //fixme
                QTimer::singleShot(500, this, SLOT(onCanHideControlCenter()));
            }
        });

        if (!m_accountUser)
            m_accountUser = new DBusAccountUser(m_userPath, this);

        if (da->isValid() && m_accountUser->isValid()) {
            DBusSessionManager sessionManager;
            QString currentUserPath = da->FindUserById(sessionManager.currentUid()).value();
            m_isCurrentUser = m_userPath == currentUserPath;

            initSegmentedControl();
            initAvatarPanel();
            initAutoLogin();
            initUserEnable();
            initAccountType();
            initPassword();

            m_mainLayout->addStretch(1);

            onAccountEnableChanged(!m_accountUser->locked());
            updateSize();
            qWarning() << "Account: init user's dbus-data completed!" << m_userPath;
        }
        else
            qWarning() << "Account: dbus's data invalid!" << m_userPath;
    }
}

void UserExpandContent::onCanHideControlCenter()
{
    this->window()->setProperty("autoHide", true);
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
    m_historyAvatarGrid = new AvatarGrid(AvatarGrid::HistoryGrid, m_userPath, this);
    m_allAvatarGrid = new AvatarGrid(AvatarGrid::NormalGrid, m_userPath, this);
    m_cameraPanel = new WebcamAvatarPanel(this);
    connect(m_historyAvatarGrid, &AvatarGrid::avatarSelected, this, &UserExpandContent::onAvatarSelected);
    connect(m_allAvatarGrid, &AvatarGrid::avatarSelected, this, &UserExpandContent::onAvatarSelected);
    connect(m_cameraPanel, &WebcamAvatarPanel::selectedAvatar, this, &UserExpandContent::onAvatarSelected);
    connect(this, &UserExpandContent::changeToSetting, [=](bool value){
        if (!value)
            m_cameraPanel->turnOffCamera();
        else if (m_stackWidget->currentIndex() == 2) {//camera stack
            m_cameraPanel->turnOnCamera();
        }
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
        m_allAvatarGrid->setAvatars(m_accountUser->iconList() << ADD_AVATAR_ICON);
        updatemAvatarGridSize(m_stackWidget->currentIndex());
        updateSize(true);
    });
    connect(m_accountUser, &DBusAccountUser::HistoryIconsChanged, [=] {
        m_historyAvatarGrid->setAvatars(m_accountUser->historyIcons());
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
    m_autoLoginLine->setTitle(tr("Auto-login"));
    m_autoLoginLine->setCheck(m_accountUser->automaticLogin());
    connect(m_autoLoginLine, &AccountSwitchLine::checkChanged, [=](bool s){
        if (m_accountUser->automaticLogin() == s)//just record the valid value(configure file just has single line record,set invalid value will cause error)
            return;

        this->window()->setProperty("autoHide", false);
        QDBusPendingReply<bool> reply = m_accountUser->SetAutomaticLogin(s);
        reply.waitForFinished();
        if (reply.error().isValid()) {
            //reset state
            m_autoLoginLine->setCheck(m_accountUser->automaticLogin());
            qWarning() << "Account: set account automatic login error: " << reply.error();
        }
    });
    connect(m_accountUser, &DBusAccountUser::AutomaticLoginChanged, [=]{
        m_autoLoginLine->setCheck(m_accountUser->automaticLogin());
    });

    m_mainLayout->addWidget(m_autoLoginLine);
}

void UserExpandContent::initUserEnable()
{
    m_lockLine = new AccountSwitchLine();
    m_lockLine->setTitle(tr("Enable"));
    m_lockLine->setCheck(!m_accountUser->locked());
    connect(m_lockLine, &AccountSwitchLine::checkChanged, [=](bool s){
        if (!m_accountUser->locked() == s)
            return;

        this->window()->setProperty("autoHide", false);
        QDBusPendingReply<bool> reply = m_accountUser->SetLocked(!s);
        reply.waitForFinished();
        if (reply.error().isValid()) {
            //reset state
            m_lockLine->setCheck(!m_accountUser->locked());
            qWarning()<<"Account: set account lock error: " << reply.error();
        }
    });
    connect(m_accountUser, &DBusAccountUser::LockedChanged, [=]{
        m_lockLine->setCheck(!m_accountUser->locked());

        onAccountEnableChanged(!m_accountUser->locked());
    });

    m_mainLayout->addWidget(m_lockLine);

    if (m_isCurrentUser) {
        m_lockLine->setFixedHeight(0);
        m_lockLine->hide();
    }
}

void UserExpandContent::initAccountType()
{
    m_typeLine = new AccountTypeLine();
    m_typeLine->setTitle(tr("Account Type"));
    m_typeLine->setType(m_accountUser->accountType());
    connect(m_typeLine, &AccountTypeLine::typeChanged, [=](int type) {
        if (type == m_accountUser->accountType())
            return;

        this->window()->setProperty("autoHide", false);
        QDBusPendingReply<bool> reply = m_accountUser->SetAccountType(type);
        reply.waitForFinished();
        if (reply.error().isValid()) {
            //reset state
            m_typeLine->setType(m_accountUser->accountType());
            qWarning() << "Account: set account type error: " << reply.error();
        }
    });
    connect(m_accountUser, &DBusAccountUser::AccountTypeChanged, [=]{
        m_typeLine->setType(m_accountUser->accountType());
    });

    m_mainLayout->addWidget(m_typeLine);

    if (m_isCurrentUser) {
        m_typeLine->setFixedHeight(0);
        m_typeLine->hide();
    }
}

void UserExpandContent::initPassword()
{
    m_passwordFrame = new PasswordFrame();
    connect(m_passwordFrame, &PasswordFrame::sizeChanged, [=]{
        updateSize();
    });
    connect(m_passwordFrame, &PasswordFrame::changePassword, [=](QString password){
        this->window()->setProperty("autoHide", false);
        QDBusPendingReply<bool> reply = m_accountUser->SetPassword(password);
        reply.waitForFinished();
        if (reply.error().isValid())
            qWarning() << "Account: set password error: " << reply.error();
    });

    m_mainLayout->addWidget(m_passwordFrame);
}

void UserExpandContent::onAvatarSelected(const QString &avatar)
{
    this->window()->setProperty("autoHide", false);
    QString fileName = "";
    if (avatar == ADD_AVATAR_ICON){
        QFileDialog dl(this);
        dl.setDirectory(QDir::homePath());
        dl.setWindowTitle(tr("Choose a new picture for your Avatar"));
        dl.setNameFilter(tr("Image files (*.jpg *.png *.jpeg)"));

        QRect rec = QApplication::desktop()->screenGeometry();
        dl.move(rec.x() + (rec.width() - dl.width()) / 2, rec.y() + (rec.height() - dl.height()) / 2);
        if (dl.exec() == QDialog::Accepted) {
            fileName = dl.selectedFiles().first();
        }
    }
    else
        fileName = avatar;

    if (!fileName.isEmpty()) {
        this->window()->setProperty("autoHide", false);
        QDBusPendingReply<bool> reply = m_accountUser->SetIconFile(fileName);
        reply.waitForFinished();
        if (reply.error().isValid())
            qWarning()<<"Account: set icon file error: " << reply.error();
    }
}

void UserExpandContent::onAccountEnableChanged(bool enabled)
{
    if (enabled) {
        m_segmentedFrame->setFixedHeight(DUI::BUTTON_HEIGHT + LAYOUT_SPACING * 2);
        updatemAvatarGridSize(m_stackWidget->currentIndex());
        m_autoLoginLine->setFixedHeight(DUI::CONTENT_HEADER_HEIGHT);
        m_passwordFrame->setFixedHeight(DUI::CONTENT_HEADER_HEIGHT);
        if (!m_isCurrentUser)
            m_typeLine->setFixedHeight(DUI::CONTENT_HEADER_HEIGHT);
    }
    else {
        m_segmentedFrame->setFixedHeight(0);
        m_stackWidget->setFixedHeight(0);
        m_autoLoginLine->setFixedHeight(0);
        m_passwordFrame->reset();
        m_passwordFrame->setFixedHeight(0);
        m_typeLine->setFixedHeight(0);
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


