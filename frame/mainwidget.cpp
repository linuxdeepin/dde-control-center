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

static const int PluginsHeightMax = 380;
static const int PluginsHeightMin = 260;

MainWidget::MainWidget(Frame *parent)
    : FrameWidget(parent),

      m_pluginsController(new PluginsController(this)),

//      m_lastPluginWidget(nullptr),

      m_timeRefersh(new QTimer(this)),

      m_userAvatarBtn(nullptr),
      m_currentTimeLbl(new QLabel),
      m_currentDateLbl(new QLabel),
      m_pluginsLayout(new QStackedLayout),
      m_indicatorWidget(new IndicatorWidget),
#ifndef DISABLE_SYS_UPDATE
      m_updateNotifier(new UpdateNotifier),
#endif
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
    m_pluginWrapper = new TranslucentFrame;
    m_pluginWrapper->setLayout(m_pluginsLayout);
    m_pluginWrapper->setObjectName("HomePluginsFrame");

    QVBoxLayout *pluginWidgetLayout = new QVBoxLayout;
    pluginWidgetLayout->addWidget(m_pluginWrapper);
    pluginWidgetLayout->addSpacing(10);
    pluginWidgetLayout->addWidget(m_indicatorWidget);
    pluginWidgetLayout->setSpacing(0);
    pluginWidgetLayout->setMargin(0);

    m_pluginWidget = new TranslucentFrame;
    m_pluginWidget->setLayout(pluginWidgetLayout);

#ifndef DISABLE_SYS_UPDATE
    m_updateNotifier->setObjectName("UpdateNotifier");
    m_updateNotifier->setVisible(false);
#endif

    QVBoxLayout *centralLayout = static_cast<QVBoxLayout *>(layout());
    centralLayout->addWidget(headerFrame);
#ifndef DISABLE_SYS_UPDATE
    centralLayout->addWidget(m_updateNotifier);
    centralLayout->addSpacing(1);
#endif
    centralLayout->addWidget(m_pluginWidget);
    centralLayout->addSpacing(20);
    centralLayout->addWidget(m_quickSettingsPanel);
    centralLayout->setSpacing(0);
    centralLayout->setMargin(0);

    connect(m_pluginsController, &PluginsController::pluginAdded, this, &MainWidget::pluginAdded, Qt::QueuedConnection);
    connect(m_pluginsController, &PluginsController::requestModulePage, this, &MainWidget::showSettingPage, Qt::QueuedConnection);
    connect(m_indicatorWidget, &IndicatorWidget::requestNext, this, &MainWidget::showPrevPlugin);
    connect(m_indicatorWidget, &IndicatorWidget::requestPrevious, this, &MainWidget::showNextPlugin);
    connect(m_quickSettingsPanel, &QuickControlPanel::requestDetailConfig, this, &MainWidget::showAllSettings);
    connect(m_quickSettingsPanel, &QuickControlPanel::requestPage, this, &MainWidget::showSettingPage);
    connect(m_timeRefersh, &QTimer::timeout, this, &MainWidget::refershTimedate);

#ifndef DISABLE_SYS_UPDATE
    connect(m_updateNotifier, &UpdateNotifier::clicked, this, [this] {
        showSettingPage("update", "available-updates");
    });
#endif

    m_pluginsController->loadPlugins();
    refershTimedate();
}

void MainWidget::resizeEvent(QResizeEvent *e)
{
    FrameWidget::resizeEvent(e);

    QTimer::singleShot(1, this, &MainWidget::updatePluginsHeight);
}

int MainWidget::getPluginsHeight()
{
    return height() > 800 ? PluginsHeightMax : PluginsHeightMin;
}

void MainWidget::updatePluginsHeight()
{
    const bool b = height() > 600;
    m_pluginWidget->setVisible(b);
    m_quickSettingsPanel->setAllSettingsVisible(!b);

    const int h = getPluginsHeight();
    m_pluginWrapper->setFixedHeight(h);
}

void MainWidget::pluginAdded(QWidget * const w)
{
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    const int idx = m_pluginsLayout->addWidget(w);
    m_pluginsLayout->setCurrentIndex(idx);
}

void MainWidget::showNextPlugin()
{
//    m_pluginsIndicator->nextPage();

    const int index = m_pluginsLayout->currentIndex();
    const int count = m_pluginsLayout->count();

    m_pluginsLayout->setCurrentIndex((index + 1) % count);
}

void MainWidget::showPrevPlugin()
{
//    m_pluginsIndicator->previousPage();

    const int index = m_pluginsLayout->currentIndex();
    const int count = m_pluginsLayout->count();

    m_pluginsLayout->setCurrentIndex((index + count - 1) % count);
}

void MainWidget::refershTimedate()
{
    const QDateTime tm = QDateTime::currentDateTime();

    m_currentTimeLbl->setText(tm.time().toString("HH:mm"));
    m_currentDateLbl->setText(tm.date().toString(Qt::SystemLocaleLongDate));
}
