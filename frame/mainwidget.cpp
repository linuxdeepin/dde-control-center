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
      m_nextPluginBtn(new QPushButton),
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

            // keep pointer
            break;
        }
        inter->deleteLater();
    }

    m_timeRefersh->setInterval(1000);
    m_timeRefersh->setSingleShot(false);
    m_timeRefersh->start();

    m_nextPluginBtn->setText("Next");

    m_currentTimeLbl->setStyleSheet("QLabel {"
                                    "color: white;"
                                    "font-size: 28px;"
                                    "font-weight: 200;"
                                    "}");
    m_currentDateLbl->setStyleSheet("QLabel {"
                                    "color: white;"
                                    "}");

    m_pluginsIndicator->setFixedHeight(20);
    m_pluginsIndicator->setPageCount(2);
    m_pluginsIndicator->setCurrentPage(0);

    QVBoxLayout *timedateLayout = new QVBoxLayout;
    timedateLayout->addWidget(m_currentTimeLbl);
    timedateLayout->addWidget(m_currentDateLbl);
    timedateLayout->setSpacing(0);
    timedateLayout->setContentsMargins(30, 0, 0, 0);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->addWidget(m_userAvatarBtn);
    headerLayout->addLayout(timedateLayout);
    headerLayout->setSpacing(0);
    headerLayout->setContentsMargins(30, 40, 0, 0);

    QVBoxLayout *centeralLayout = static_cast<QVBoxLayout *>(layout());
    centeralLayout->addLayout(headerLayout);
    centeralLayout->addLayout(m_pluginsLayout);
    centeralLayout->addWidget(m_pluginsIndicator);
    centeralLayout->addWidget(m_nextPluginBtn);
    centeralLayout->addStretch();
    centeralLayout->addWidget(m_quickSettingsPanel);
    centeralLayout->setSpacing(0);
    centeralLayout->setMargin(0);

    connect(m_pluginsController, &PluginsController::pluginAdded, this, &MainWidget::pluginAdded);
    connect(m_nextPluginBtn, &QPushButton::clicked, this, &MainWidget::showNextPlugin);
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

void MainWidget::refershTimedate()
{
    const QDateTime tm = QDateTime::currentDateTime();

    m_currentTimeLbl->setText(tm.toString("HH:mm"));
    m_currentDateLbl->setText(tm.toString(Qt::SystemLocaleShortDate));
}
