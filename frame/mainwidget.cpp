#include "mainwidget.h"

#include "accounts/avatarwidget.h"

#include <dimagebutton.h>

#include <com_deepin_daemon_accounts.h>
#include <com_deepin_daemon_accounts_user.h>

#include <unistd.h>

using namespace dcc::accounts;
using AccountsInter = com::deepin::daemon::Accounts;
using UserInter = com::deepin::daemon::accounts::User;

DWIDGET_USE_NAMESPACE

const int PLUGINS_HEIGHT = 400;

MainWidget::MainWidget(Frame *parent)
    : FrameWidget(parent),

      m_pluginsController(new PluginsController(this)),

      m_lastPluginWidget(nullptr),

      m_timeRefersh(new QTimer(this)),

      m_userAvatarBtn(nullptr),
      m_currentTimeLbl(new QLabel),
      m_currentDateLbl(new QLabel),
      m_pluginsLayout(new QHBoxLayout),
      m_pluginsIndicator(new DPageIndicator),
      m_nextPluginBtn(new Dtk::Widget::DImageButton),
      m_prevPluginBtn(new Dtk::Widget::DImageButton),
      m_quickSettingsPanel(new QuickControlPanel)
{
    // TODO: get dbus data
    const int uid = getuid();
    AccountsInter accountsInter("com.deepin.daemon.Accounts", "/com/deepin/daemon/Accounts", QDBusConnection::systemBus(), this);
    for (auto user : accountsInter.userList())
    {
        UserInter *inter = new UserInter("com.deepin.daemon.Accounts", user, QDBusConnection::systemBus(), this);
        if (inter->uid().toInt() == uid)
        {
            m_userAvatarBtn = new AvatarWidget(inter->iconFile());

            connect(inter, &UserInter::IconFileChanged, m_userAvatarBtn, &AvatarWidget::setAvatarPath);
            connect(m_userAvatarBtn, &AvatarWidget::clicked, this, &MainWidget::showAccountsModule);

            // keep pointer
            break;
        }
        inter->deleteLater();
    }

    m_timeRefersh->setInterval(1000);
    m_timeRefersh->setSingleShot(false);
    m_timeRefersh->start();

    m_prevPluginBtn->setObjectName("PrevBtn");
    m_prevPluginBtn->setAccessibleName("PrevBtn");
    m_nextPluginBtn->setObjectName("NextBtn");
    m_nextPluginBtn->setAccessibleName("NextBtn");
    m_pluginsIndicator->setObjectName("DPIndicator");

    QFont font = m_currentTimeLbl->font();
    font.setPixelSize(46);
    font.setWeight(QFont::Light);

    m_currentTimeLbl->setObjectName("CurrentTimeLabel");
    m_currentTimeLbl->setFont(font);
    m_currentDateLbl->setObjectName("CurrentDateLabel");

    m_pluginsIndicator->setFixedHeight(20);
    m_pluginsIndicator->setPageCount(2);
    m_pluginsIndicator->setCurrentPage(0);
    m_pluginsIndicator->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *indicatorLayout = new QHBoxLayout;
    indicatorLayout->addWidget(m_prevPluginBtn);
    indicatorLayout->addWidget(m_pluginsIndicator);
    indicatorLayout->addWidget(m_nextPluginBtn);
    indicatorLayout->setContentsMargins(10, 0, 10, 0);

    // Header
    TranslucentFrame *headerFrame = new TranslucentFrame;
    headerFrame->setFixedHeight(140);
    headerFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QVBoxLayout *timedateLayout = new QVBoxLayout;
    timedateLayout->setSpacing(0);
    timedateLayout->setMargin(0);
    timedateLayout->addStretch();
    timedateLayout->addWidget(m_currentTimeLbl);
    timedateLayout->addWidget(m_currentDateLbl);
    timedateLayout->addStretch();

    QVBoxLayout *avatarLayout = new QVBoxLayout;
    avatarLayout->setSpacing(0);
    avatarLayout->setMargin(0);
    avatarLayout->addStretch();
    avatarLayout->addSpacing(20);
    avatarLayout->addWidget(m_userAvatarBtn);
    avatarLayout->addStretch();

    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->addLayout(avatarLayout);
    headerLayout->addSpacing(30);
    headerLayout->addLayout(timedateLayout);
    headerLayout->setSpacing(0);
    headerLayout->setMargin(0);
    headerLayout->setContentsMargins(30, 0, 30, 20);

    headerFrame->setLayout(headerLayout);

    // Plugins
    QFrame *frame = new QFrame;
    frame->setObjectName("HomePluginsFrame");
    frame->setLayout(m_pluginsLayout);

    QVBoxLayout *centeralLayout = static_cast<QVBoxLayout *>(layout());
    centeralLayout->addWidget(headerFrame);
    centeralLayout->addWidget(frame);
    centeralLayout->addSpacing(20);
    centeralLayout->addLayout(indicatorLayout);
//    centeralLayout->addStretch();
    centeralLayout->addWidget(m_quickSettingsPanel);
    centeralLayout->setSpacing(0);
    centeralLayout->setMargin(0);

    connect(m_pluginsController, &PluginsController::pluginAdded, this, &MainWidget::pluginAdded);
    connect(m_prevPluginBtn, &DImageButton::clicked, this, &MainWidget::showPrevPlugin);
    connect(m_nextPluginBtn, &DImageButton::clicked, this, &MainWidget::showNextPlugin);
    connect(m_quickSettingsPanel, &QuickControlPanel::requestDetailConfig, this, &MainWidget::showAllSettings);
    connect(m_timeRefersh, &QTimer::timeout, this, &MainWidget::refershTimedate);

    m_pluginsController->loadPlugins();
    refershTimedate();
}

void MainWidget::showPlugin(QWidget * const w)
{
    if (m_lastPluginWidget)
        m_lastPluginWidget->setVisible(false);
    m_lastPluginWidget = w;
    m_lastPluginWidget->setVisible(true);
}

void MainWidget::pluginAdded(QWidget * const w)
{
    w->setFixedHeight(PLUGINS_HEIGHT);
    m_pluginsLayout->addWidget(w);
    showPlugin(w);
}

void MainWidget::showNextPlugin()
{
    m_pluginsIndicator->nextPage();

    const int index = m_pluginsLayout->indexOf(m_lastPluginWidget);
    QLayoutItem *item = m_pluginsLayout->itemAt(index + 1);
    if (item && item->widget())
        showPlugin(item->widget());
    else
        showPlugin(m_pluginsLayout->itemAt(0)->widget());
}

void MainWidget::showPrevPlugin()
{
    m_pluginsIndicator->previousPage();

    const int index = m_pluginsLayout->indexOf(m_lastPluginWidget);
    const int count = m_pluginsLayout->count();

    if (index == 0)
        showPlugin(m_pluginsLayout->itemAt(count - 1)->widget());
    else
        showPlugin(m_pluginsLayout->itemAt(index - 1)->widget());
}

void MainWidget::refershTimedate()
{
    const QDateTime tm = QDateTime::currentDateTime();

    m_currentTimeLbl->setText(tm.time().toString("HH:mm"));
    m_currentDateLbl->setText(tm.date().toString(Qt::SystemLocaleLongDate));
}

void MainWidget::showAccountsModule()
{
    emit showSettingPage("accounts", QString());
}
