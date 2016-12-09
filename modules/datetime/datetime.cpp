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

namespace dcc {
namespace datetime {

Datetime::Datetime()
    :ModuleWidget(),
      m_bEdit(false)
{
    setObjectName("Datetime");

    m_settings = new QSettings("dde-control-center", "datetime", this);

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

    connect(m_addItem, SIGNAL(clicked()), this, SIGNAL(addClick()));
    connect(timeItem, SIGNAL(clicked()), this, SIGNAL(editDatetime()));
    connect(m_headItem, SIGNAL(editChanged(bool)), this, SLOT(slotEditMode(bool)));

    QStringList groups = m_settings->childGroups();
    for(int i = 0; i<groups.count(); i++)
    {
        m_settings->beginGroup(groups.at(i));
        Timezone tz;
        tz.m_city = m_settings->value("City").toString();
        tz.m_timezone = m_settings->value("Timezone").toString();
        m_settings->endGroup();
        this->addTimezone(tz);
    }
}

Datetime::~Datetime()
{
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

    m_settings->beginGroup(tz.m_city);
    m_settings->setValue("City", tz.m_city);
    m_settings->setValue("Timezone", tz.m_timezone);
    m_settings->endGroup();
}

void Datetime::slotClick()
{
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
        m_settings->remove(tz.m_city+"/"+"City");
        m_settings->remove(tz.m_city+"/"+"Timezone");
    }
}
}
}
