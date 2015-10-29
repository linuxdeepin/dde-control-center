#include "applictionitemwidget.h"
#include "constants.h"

#include <QVBoxLayout>
#include <QPixmap>

#include <libdui/dthememanager.h>

ApplictionItemWidget::ApplictionItemWidget(QWidget *parent)
    : QFrame(parent)
{
    D_THEME_INIT_WIDGET(ApplictionItemWidget, selected, hovered);

    m_dbusUpdateInter = new DBusLastoreManager("org.deepin.lastore", "/org/deepin/lastore", QDBusConnection::systemBus(), this);

    m_appIcon = new QLabel;
    m_appName = new QLabel;
    m_appName->setObjectName("AppName");
    m_appVersion = new QLabel;
    m_appVersion->setObjectName("AppVersion");
    m_progress = new DCircleProgress;
    m_progress->setFixedSize(25, 25);
    m_progress->setLineWidth(2);
    m_progress->setValue(0);
    m_progress->hide();
    m_updateBtn = new QPushButton;
    m_updateBtn->setText(tr("Update"));
    m_updateBtn->setObjectName("UpdateButton");
    m_updateBtn->hide();

    // TODO: remove
    m_appIcon->setPixmap(QPixmap(":/images/images/select_active.png"));
    m_appName->setText("app name");
    m_appVersion->setText("app version");

    QVBoxLayout *infoLayout = new QVBoxLayout;
    infoLayout->addStretch();
    infoLayout->addWidget(m_appName);
    infoLayout->addWidget(m_appVersion);
    infoLayout->addStretch();
    infoLayout->setSpacing(0);
    infoLayout->setMargin(0);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_appIcon);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(infoLayout);
    mainLayout->addStretch();
    mainLayout->addWidget(m_progress);
    mainLayout->addWidget(m_updateBtn);
    mainLayout->addSpacing(25);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    setFixedHeight(50);
    setFixedWidth(DCC::ModuleContentWidth);
    setLayout(mainLayout);

    connect(m_updateBtn, &QPushButton::clicked, this, &ApplictionItemWidget::toggleUpdateJob);
}

void ApplictionItemWidget::setAppUpdateInfo(const AppUpdateInfo &info)
{
     m_updateInfo = info;

     qDebug() << info;

     m_appName->setText(info.m_name);
     m_appVersion->setText(QString("%1 -> %2").arg(info.m_currentVersion)
                                            .arg(info.m_avilableVersion));
}

void ApplictionItemWidget::enterEvent(QEvent *)
{
    if (!m_progress->isVisible())
        m_updateBtn->show();
    m_hovered = true;
    emit hoverStateChanged();
}

void ApplictionItemWidget::leaveEvent(QEvent *)
{
    m_updateBtn->hide();
    m_hovered = false;
    emit hoverStateChanged();
}

void ApplictionItemWidget::toggleUpdateJob()
{
    m_jobRunning = !m_jobRunning;
    qDebug() << "toggle running = " << m_jobRunning;

    m_updateBtn->setVisible(!m_jobRunning);
    m_progress->setVisible(m_jobRunning);
}

