#include "datetime.h"
#include "contentwidget.h"
#include "timezoneitem.h"
#include "datesettings.h"
#include "nextpagewidget.h"
#include "switchwidget.h"
#include "settingshead.h"

#include <QFrame>
#include <QDebug>
#include <QPushButton>

Datetime::Datetime()
    :ModuleWidget(),
      m_choseDlg(new ChoseDialog())
{
    this->installEventFilter(parent());
    setTitle(tr("Time and Date"));
    Clock* clock = new Clock();
    clock->setDisplay(true);
    SettingsGroup* clockGroup = new SettingsGroup();
    clockGroup->appendItem(clock);
    m_centeralLayout->addWidget(clockGroup);

    m_group = new SettingsGroup();
    m_headItem = new SettingsHead();
    m_headItem->setTitle(tr("Timezone List"));
    m_group->appendItem(m_headItem);
    m_centeralLayout->addSpacerItem(new QSpacerItem(300,20));
    m_centeralLayout->addWidget(m_group);

    SettingsGroup* addTimezone = new SettingsGroup();
    m_addItem = new NextPageWidget();
    m_addItem->setTitle(tr("Add Timezone"));
    addTimezone->appendItem(m_addItem);

    SettingsGroup* timeSettings = new SettingsGroup();
    NextPageWidget* timeItem = new NextPageWidget();
    timeItem->setTitle(tr("Time Settings"));
    timeSettings->appendItem(timeItem);
//    timeSettings->appendItem(new SwitchWidget());

    m_centeralLayout->addSpacerItem(new QSpacerItem(300,20));
    m_centeralLayout->addWidget(addTimezone);
    m_centeralLayout->addSpacerItem(new QSpacerItem(300,20));
    m_centeralLayout->addWidget(timeSettings);

    connect(m_addItem, SIGNAL(clicked()), this, SLOT(slotClick()));
    connect(timeItem, SIGNAL(clicked()), this, SIGNAL(editDatetime()));
    connect(m_headItem, SIGNAL(editChanged(bool)), this, SLOT(slotEditMode(bool)));
    connect(m_choseDlg, SIGNAL(addTimezone(Timezone)), this, SLOT(addTimezone(Timezone)));
}

Datetime::~Datetime()
{
    m_choseDlg->deleteLater();
}

void Datetime::addTimezone(const Timezone &tz)
{
    if(m_addeds.contains(tz))
    {
        return;
    }
    TimezoneItem* item = new TimezoneItem();
    connect(m_headItem, SIGNAL(editChanged(bool)), item, SLOT(slotStatus(bool)));
    connect(item, SIGNAL(destroySelf()), m_headItem, SLOT(initStatus()));
    connect(item, SIGNAL(removeTimezone(Timezone)), this, SLOT(slotRemoveTimezone(Timezone)));

    item->setCity(tz);
    item->setFixedHeight(40);
    item->slotStatus(m_bEdit);
    m_group->appendItem(item);
    m_addeds.append(tz);
}

void Datetime::slotClick()
{
    m_choseDlg->show();
}

void Datetime::slotEditMode(bool edit)
{
    m_bEdit = edit;
    m_addItem->setEnabled(!m_bEdit);
}

void Datetime::slotRemoveTimezone(const Timezone &tz)
{
    TimezoneItem* item = qobject_cast<TimezoneItem*>(sender());
    if(item)
    {
        m_addeds.removeOne(tz);
        m_group->removeItem(item);
        item->deleteLater();
    }
}
