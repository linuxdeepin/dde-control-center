#include "applictionitemwidget.h"
#include "constants.h"

#include <QVBoxLayout>
#include <QPixmap>

#include <libdui/dthememanager.h>

ApplictionItemWidget::ApplictionItemWidget(QWidget *parent)
    : QWidget(parent)
{
    D_THEME_INIT_WIDGET(ApplictionItemWidget, selected);

    m_appIcon = new QLabel;
    m_appName = new QLabel;
    m_appName->setObjectName("AppName");
    m_appVersion = new QLabel;
    m_appVersion->setObjectName("AppVersion");
    m_updateBtn = new DImageButton;
    m_updateBtn->hide();

    // TODO: remove
    m_appIcon->setPixmap(QPixmap(":/images/images/select_active.png"));
    m_appName->setText("app name");
    m_appVersion->setText("app version");
    m_updateBtn->setNormalPic(":/images/images/select_active.png");

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
    mainLayout->addWidget(m_updateBtn);
    mainLayout->addSpacing(25);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    setFixedHeight(50);
    setFixedWidth(DCC::ModuleContentWidth);
    setLayout(mainLayout);
}

void ApplictionItemWidget::enterEvent(QEvent *)
{
    m_updateBtn->show();
}

void ApplictionItemWidget::leaveEvent(QEvent *)
{
    m_updateBtn->hide();
}

