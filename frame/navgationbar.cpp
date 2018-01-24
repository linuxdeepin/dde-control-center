#include "navgationbar.h"

#include <QVBoxLayout>

NavgationBar::NavgationBar(QWidget *parent)
    : QWidget(parent)

    , m_accountBtn(new DImageButton)
    , m_displayBtn(new DImageButton)
    , m_defaultAppsBtn(new DImageButton)
    , m_personalizationBtn(new DImageButton)
    , m_networkBtn(new DImageButton)
    , m_bluetoothBtn(new DImageButton)
    , m_soundBtn(new DImageButton)
    , m_timeBtn(new DImageButton)
    , m_powerBtn(new DImageButton)
    , m_mouseBtn(new DImageButton)
    , m_keyboardBtn(new DImageButton)
    , m_updateBtn(new DImageButton)
    , m_sysInfoBtn(new DImageButton)
{
    m_accountBtn->setNormalPic(":/accounts/themes/dark/icons/nav_accounts.png");
    m_displayBtn->setNormalPic(":/display/themes/dark/icons/nav_display.png");
    m_defaultAppsBtn->setNormalPic(":/defapp/themes/dark/icons/nav_defapp.png");
    m_personalizationBtn->setNormalPic(":/personalization/themes/dark/icons/nav_personalization.png");
    m_networkBtn->setNormalPic(":/network/themes/dark/icons/nav_network.png");
    m_bluetoothBtn->setNormalPic(":/bluetooth/themes/dark/icons/nav_bluetooth.png");
    m_soundBtn->setNormalPic(":/sound/themes/dark/icons/nav_sound.png");
    m_timeBtn->setNormalPic(":/datetime/themes/dark/icons/nav_datetime.png");
    m_powerBtn->setNormalPic(":/power/themes/dark/icons/nav_power.png");
    m_mouseBtn->setNormalPic(":/mouse/themes/dark/icons/nav_mouse.png");
    m_keyboardBtn->setNormalPic(":/keyboard/themes/dark/icons/nav_keyboard.png");
    m_updateBtn->setNormalPic(":/update/themes/dark/icons/nav_update.png");
    m_sysInfoBtn->setNormalPic(":/systeminfo/themes/dark/icons/nav_systeminfo.png");

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();
    centralLayout->addWidget(m_accountBtn);
    centralLayout->addWidget(m_displayBtn);
    centralLayout->addWidget(m_defaultAppsBtn);
    centralLayout->addWidget(m_personalizationBtn);
    centralLayout->addWidget(m_networkBtn);
    centralLayout->addWidget(m_bluetoothBtn);
    centralLayout->addWidget(m_soundBtn);
    centralLayout->addWidget(m_timeBtn);
    centralLayout->addWidget(m_powerBtn);
    centralLayout->addWidget(m_mouseBtn);
    centralLayout->addWidget(m_keyboardBtn);
    centralLayout->addWidget(m_updateBtn);
    centralLayout->addWidget(m_sysInfoBtn);
    centralLayout->addStretch();
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(20);

    setLayout(centralLayout);
}
