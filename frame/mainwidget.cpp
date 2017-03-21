#include "mainwidget.h"

#include "accounts/avatarwidget.h"
#include "updatenotifier.h"

#include <dimagebutton.h>

#include <com_deepin_daemon_accounts.h>
#include <com_deepin_daemon_accounts_user.h>

#include <unistd.h>

using namespace dcc::accounts;
using AccountsInter = com::deepin::daemon::Accounts;
using UserInter = com::deepin::daemon::accounts::User;

DWIDGET_USE_NAMESPACE

const int PLUGINS_HEIGHT = 380;

MainWidget::MainWidget(Frame *parent)
    : FrameWidget(parent),

      m_pluginsController(new PluginsController(this)),

      m_lastPluginWidget(nullptr),

      m_timeRefersh(new QTimer(this)),

      m_userAvatarBtn(nullptr),
      m_currentTimeLbl(new QLabel),
      m_currentDateLbl(new QLabel),
      m_pluginsLayout(new QHBoxLayout),
      m_indicatorWidget(new IndicatorWidget),
      m_updateNotifier(new UpdateNotifier),
      m_quickSettingsPanel(new QuickControlPanel)
{
    m_pluginsLayout->setMargin(0);
    m_pluginsLayout->setSpacing(0);

    // TODO: get dbus data
    const int uid = getuid();
    AccountsInter accountsInter("com.deepin.daemon.Accounts", "/com/deepin/daemon/Accounts", QDBusConnection::systemBus(), this);
    for (auto user : accountsInter.userList())
    {
        UserInter *inter = new UserInter("com.deepin.daemon.Accounts", user, QDBusConnection::systemBus(), this);
        if (inter->uid().toInt() == uid)
        {
            m_userAvatarBtn = new AvatarWidget(inter->iconFile());
            m_userAvatarBtn->setAccessibleName(inter->iconFile());
            m_userAvatarBtn->setAccessibleDescription("iconMainWidget");

            connect(inter, &UserInter::IconFileChanged, m_userAvatarBtn, &AvatarWidget::setAvatarPath);
            connect(m_userAvatarBtn, &AvatarWidget::clicked, this, [=] { emit showSettingPage("accounts", inter->userName()); });

            // keep pointer
            break;
        }
        inter->deleteLater();
    }

    m_timeRefersh->setInterval(1000);
    m_timeRefersh->setSingleShot(false);
    m_timeRefersh->start();

    QFont font = m_currentTimeLbl->font();
    font.setPixelSize(46);
    font.setWeight(QFont::Light);

    m_currentTimeLbl->setObjectName("CurrentTimeLabel");
    m_currentTimeLbl->setFont(font);
    m_currentDateLbl->setObjectName("CurrentDateLabel");

//    m_pluginsIndicator->setFixedHeight(20);
//    m_pluginsIndicator->setPageCount(2);
//    m_pluginsIndicator->setCurrentPage(0);
//    m_pluginsIndicator->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
//    m_pluginsIndicator->setVisible(false);

//    QHBoxLayout *indicatorLayout = new QHBoxLayout;
//    indicatorLayout->addStretch();
//    indicatorLayout->addWidget(m_prevPluginBtn);
//    indicatorLayout->addSpacing(15);
//    indicatorLayout->addWidget(m_pluginsIndicator);
//    indicatorLayout->addSpacing(15);
//    indicatorLayout->addWidget(m_nextPluginBtn);
//    indicatorLayout->addStretch();
//    indicatorLayout->setContentsMargins(10, 0, 10, 0);

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
    QWidget *pluginWrapper = new TranslucentFrame;
    pluginWrapper->setLayout(m_pluginsLayout);
    pluginWrapper->setObjectName("HomePluginsFrame");

    QVBoxLayout *pluginWidgetLayout = new QVBoxLayout;
    pluginWidgetLayout->addWidget(pluginWrapper);
    pluginWidgetLayout->addSpacing(10);
    pluginWidgetLayout->addWidget(m_indicatorWidget);
    pluginWidgetLayout->setSpacing(0);
    pluginWidgetLayout->setMargin(0);

    m_pluginWidget = new TranslucentFrame;
    m_pluginWidget->setLayout(pluginWidgetLayout);

    m_updateNotifier->setObjectName("UpdateNotifier");
    m_updateNotifier->setVisible(false);

    QVBoxLayout *centralLayout = static_cast<QVBoxLayout *>(layout());
    centralLayout->addWidget(headerFrame);
    centralLayout->addWidget(m_updateNotifier);
    centralLayout->addSpacing(1);
    centralLayout->addWidget(m_pluginWidget);
    centralLayout->addSpacing(20);
    centralLayout->addWidget(m_quickSettingsPanel);
    centralLayout->setSpacing(0);
    centralLayout->setMargin(0);

    connect(m_pluginsController, &PluginsController::pluginAdded, this, &MainWidget::pluginAdded);
    connect(m_indicatorWidget, &IndicatorWidget::requestNext, this, &MainWidget::showNextPlugin);
    connect(m_indicatorWidget, &IndicatorWidget::requestPrevious, this, &MainWidget::showPrevPlugin);
//    connect(m_prevPluginBtn, &DImageButton::clicked, this, &MainWidget::showPrevPlugin);
//    connect(m_nextPluginBtn, &DImageButton::clicked, this, &MainWidget::showNextPlugin);
    connect(m_quickSettingsPanel, &QuickControlPanel::requestDetailConfig, this, &MainWidget::showAllSettings);
    connect(m_quickSettingsPanel, &QuickControlPanel::requestPage, this, &MainWidget::showSettingPage);
    connect(m_timeRefersh, &QTimer::timeout, this, &MainWidget::refershTimedate);

    m_pluginsController->loadPlugins();
    refershTimedate();
}

void MainWidget::resizeEvent(QResizeEvent *e)
{
    FrameWidget::resizeEvent(e);

    // hide plugins area if panel too low
    m_pluginWidget->setVisible(e->size().height() > 600);
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
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    w->setFixedHeight(PLUGINS_HEIGHT);
    m_pluginsLayout->addWidget(w);
    showPlugin(w);
}

void MainWidget::showNextPlugin()
{
//    m_pluginsIndicator->nextPage();

    const int index = m_pluginsLayout->indexOf(m_lastPluginWidget);
    QLayoutItem *item = m_pluginsLayout->itemAt(index + 1);
    if (item && item->widget())
        showPlugin(item->widget());
    else
        showPlugin(m_pluginsLayout->itemAt(0)->widget());
}

void MainWidget::showPrevPlugin()
{
//    m_pluginsIndicator->previousPage();

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
