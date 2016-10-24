#include "accounts.h"

#include "settingsgroup.h"
#include "settingsitem.h"
#include "nextpagewidget.h"
#include "switchwidget.h"

#include <QPushButton>

using namespace dcc;

Accounts::Accounts(dde::FrameProxyInterface *frame)
    : ModuleWidget(frame)
{

    QLabel *avatarLabel = new QLabel;
    avatarLabel->setText("Avatar");

    QHBoxLayout *avatarLayout = new QHBoxLayout;
    avatarLayout->addWidget(avatarLabel);

    SettingsItem *avatarItem = new SettingsItem;
    avatarItem->setLayout(avatarLayout);

    QLabel *pwdLbl = new QLabel;
    pwdLbl->setText("Pwd");

    QHBoxLayout *pwdLayout = new QHBoxLayout;
    pwdLayout->addWidget(pwdLbl);

    SettingsItem *passwordItem = new SettingsItem;
    passwordItem->setLayout(pwdLayout);

    SettingsGroup *basicSettings = new SettingsGroup;
    basicSettings->appendItem(avatarItem);
    basicSettings->appendItem(passwordItem);
    basicSettings->appendItem(new NextPageWidget);
    basicSettings->appendItem(new SwitchWidget);

    m_centeralLayout->addWidget(basicSettings);

    m_nextPage = new QPushButton;
    m_nextPage->setText("Next Page >");
    m_centeralLayout->addWidget(m_nextPage);

    QLabel *lbl = new QLabel;
    lbl->setText("account detail widget");
    lbl->setStyleSheet("background-color:gray;");
    m_detail = new ContentWidget;
    m_detail->setTitle("account detail");
    m_detail->setContent(lbl);

    for (int i(0); i != 20; ++i)
    {
        QPushButton *b = new QPushButton;
        b->setText("test " + QString::number(i));
        m_centeralLayout->addWidget(b);
    }

    setTitle(tr("Accounts"));

    connect(m_nextPage, &QPushButton::clicked, [this] { pushWidget(m_detail); });
}
