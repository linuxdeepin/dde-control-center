#include "keyboardwidget.h"
#include "settingsgroup.h"
#include "nextpagewidget.h"

using namespace dcc;

KeyboardWidget::KeyboardWidget()
    :ModuleWidget()
{
    this->installEventFilter(parent());
    setTitle(tr("Keyboard and Language"));

    SettingsGroup* keyGroup = new SettingsGroup();
    m_keyItem = new NextPageWidget();
    m_keyItem->setTitle(tr("Keyboard Layout"));
    m_keyItem->setValue(tr(""));
    keyGroup->appendItem(m_keyItem);

    SettingsGroup* langGroup = new SettingsGroup();
    m_langItem = new NextPageWidget();
    m_langItem->setTitle(tr("System Language"));
    m_langItem->setValue(tr("Simplified Chinese"));
    langGroup->appendItem(m_langItem);

    SettingsGroup* scGroup = new SettingsGroup();
    m_scItem = new NextPageWidget();
    m_scItem->setTitle(tr("Shortcut"));
    scGroup->appendItem(m_scItem);

    m_centeralLayout->setSpacing(20);
    m_centeralLayout->addWidget(keyGroup);
    m_centeralLayout->addWidget(langGroup);
    m_centeralLayout->addWidget(scGroup);

    connect(m_keyItem,SIGNAL(clicked()), this, SIGNAL(keyoard()));
    connect(m_langItem, SIGNAL(clicked()), this, SIGNAL(language()));
    connect(m_scItem, SIGNAL(clicked()), this, SIGNAL(shortcut()));
}
