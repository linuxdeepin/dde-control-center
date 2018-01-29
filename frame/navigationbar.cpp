#include "navigationbar.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QSignalMapper>

NavigationBar::NavigationBar(QWidget *parent)
    : QWidget(parent)

    , m_navigationsGroup(new QButtonGroup)

    , m_accountBtn(new QPushButton)
    , m_displayBtn(new QPushButton)
    , m_defaultAppsBtn(new QPushButton)
    , m_personalizationBtn(new QPushButton)
    , m_networkBtn(new QPushButton)
    , m_bluetoothBtn(new QPushButton)
    , m_soundBtn(new QPushButton)
    , m_timeBtn(new QPushButton)
    , m_powerBtn(new QPushButton)
    , m_mouseBtn(new QPushButton)
    , m_keyboardBtn(new QPushButton)
    , m_updateBtn(new QPushButton)
    , m_sysInfoBtn(new QPushButton)
{
    m_accountBtn->setIcon(QIcon(":/accounts/themes/dark/icons/nav_accounts.png"));
    m_accountBtn->setCheckable(true);
    m_displayBtn->setIcon(QIcon(":/display/themes/dark/icons/nav_display.png"));
    m_displayBtn->setCheckable(true);
    m_defaultAppsBtn->setIcon(QIcon(":/defapp/themes/dark/icons/nav_defapp.png"));
    m_defaultAppsBtn->setCheckable(true);
    m_personalizationBtn->setIcon(QIcon(":/personalization/themes/dark/icons/nav_personalization.png"));
    m_personalizationBtn->setCheckable(true);
    m_networkBtn->setIcon(QIcon(":/network/themes/dark/icons/nav_network.png"));
    m_networkBtn->setCheckable(true);
    m_bluetoothBtn->setIcon(QIcon(":/bluetooth/themes/dark/icons/nav_bluetooth.png"));
    m_bluetoothBtn->setCheckable(true);
    m_soundBtn->setIcon(QIcon(":/sound/themes/dark/icons/nav_sound.png"));
    m_soundBtn->setCheckable(true);
    m_timeBtn->setIcon(QIcon(":/datetime/themes/dark/icons/nav_datetime.png"));
    m_timeBtn->setCheckable(true);
    m_powerBtn->setIcon(QIcon(":/power/themes/dark/icons/nav_power.png"));
    m_powerBtn->setCheckable(true);
    m_mouseBtn->setIcon(QIcon(":/mouse/themes/dark/icons/nav_mouse.png"));
    m_mouseBtn->setCheckable(true);
    m_keyboardBtn->setIcon(QIcon(":/keyboard/themes/dark/icons/nav_keyboard.png"));
    m_keyboardBtn->setCheckable(true);
    m_updateBtn->setIcon(QIcon(":/update/themes/dark/icons/nav_update.png"));
    m_updateBtn->setCheckable(true);
    m_sysInfoBtn->setIcon(QIcon(":/systeminfo/themes/dark/icons/nav_systeminfo.png"));
    m_sysInfoBtn->setCheckable(true);

    m_navigationsGroup->addButton(m_accountBtn);
    m_navigationsGroup->addButton(m_displayBtn);
    m_navigationsGroup->addButton(m_defaultAppsBtn);
    m_navigationsGroup->addButton(m_personalizationBtn);
    m_navigationsGroup->addButton(m_networkBtn);
    m_navigationsGroup->addButton(m_bluetoothBtn);
    m_navigationsGroup->addButton(m_soundBtn);
    m_navigationsGroup->addButton(m_timeBtn);
    m_navigationsGroup->addButton(m_powerBtn);
    m_navigationsGroup->addButton(m_mouseBtn);
    m_navigationsGroup->addButton(m_keyboardBtn);
    m_navigationsGroup->addButton(m_updateBtn);
    m_navigationsGroup->addButton(m_sysInfoBtn);
    m_navigationsGroup->setExclusive(true);

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

    QSignalMapper *moduleMapper = new QSignalMapper(this);
    moduleMapper->setMapping(m_accountBtn, "accounts");
    moduleMapper->setMapping(m_displayBtn, "display");
    moduleMapper->setMapping(m_defaultAppsBtn, "defapp");
    moduleMapper->setMapping(m_personalizationBtn, "personalization");
    moduleMapper->setMapping(m_networkBtn, "network");
    moduleMapper->setMapping(m_bluetoothBtn, "bluetooth");
    moduleMapper->setMapping(m_soundBtn, "sound");
    moduleMapper->setMapping(m_timeBtn, "datetime");
    moduleMapper->setMapping(m_powerBtn, "power");
    moduleMapper->setMapping(m_mouseBtn, "mouse");
    moduleMapper->setMapping(m_keyboardBtn, "keyboard");
    moduleMapper->setMapping(m_updateBtn, "update");
    moduleMapper->setMapping(m_sysInfoBtn, "systeminfo");

    connect(moduleMapper, static_cast<void (QSignalMapper::*)(const QString &)>(&QSignalMapper::mapped), this, &NavigationBar::requestModule);
    connect(m_accountBtn, &QPushButton::clicked, moduleMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(m_displayBtn, &QPushButton::clicked, moduleMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(m_defaultAppsBtn, &QPushButton::clicked, moduleMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(m_personalizationBtn, &QPushButton::clicked, moduleMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(m_networkBtn, &QPushButton::clicked, moduleMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(m_bluetoothBtn, &QPushButton::clicked, moduleMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(m_soundBtn, &QPushButton::clicked, moduleMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(m_timeBtn, &QPushButton::clicked, moduleMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(m_powerBtn, &QPushButton::clicked, moduleMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(m_mouseBtn, &QPushButton::clicked, moduleMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(m_keyboardBtn, &QPushButton::clicked, moduleMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(m_updateBtn, &QPushButton::clicked, moduleMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    connect(m_sysInfoBtn, &QPushButton::clicked, moduleMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
}
