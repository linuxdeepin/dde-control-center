#include "datetime.h"

#include <QFrame>
#include <QDebug>
#include <QPushButton>

#include "contentwidget.h"
#include "timezoneitem.h"
#include "datesettings.h"
#include "nextpagewidget.h"
#include "switchwidget.h"
#include "settingshead.h"

#include "datetimemodel.h"

namespace dcc {
namespace datetime {

Datetime::Datetime()
    : ModuleWidget(),
      m_bEdit(false),
      m_model(nullptr),
      m_timeSettingsGroup(new SettingsGroup),
      m_ntpSwitch(new SwitchWidget(tr("Auto-­Sync"))),
      m_timePageButton(new NextPageWidget),
      m_timezoneGroup(new SettingsGroup),
      m_headItem(new SettingsHead),
      m_addTimezoneButton(new QPushButton(tr("Add Timezone")))
{
    setObjectName("Datetime");
    setTitle(tr("Time and Date"));

    this->installEventFilter(parent());

    SettingsGroup *clockGroup = new SettingsGroup;
    Clock *clock = new Clock;
    clock->setDisplay(true);
    clockGroup->appendItem(clock);

    m_timePageButton->setTitle(tr("Time Settings"));
    m_timeSettingsGroup->appendItem(m_ntpSwitch);
    m_timeSettingsGroup->appendItem(m_timePageButton);

    m_headItem->setTitle(tr("Timezone List"));

    m_timezoneGroup->appendItem(m_headItem);

    m_centeralLayout->addWidget(clockGroup);
    m_centeralLayout->addSpacing(10);
    m_centeralLayout->addWidget(m_timeSettingsGroup);
    m_centeralLayout->addSpacing(10);
    m_centeralLayout->addWidget(m_timezoneGroup);
    m_centeralLayout->addSpacing(10);
    m_centeralLayout->addWidget(m_addTimezoneButton);

    connect(m_ntpSwitch, &SwitchWidget::checkedChanegd, this, &Datetime::requestSetNtp);
    connect(m_timePageButton, &NextPageWidget::clicked, this, &Datetime::requestTimeSettings);
}

Datetime::~Datetime()
{
}

void Datetime::setModel(const DatetimeModel *model)
{
    m_model = model;

    connect(model, &DatetimeModel::userTimeZoneAdded, this, &Datetime::addTimezone);
    connect(model, &DatetimeModel::userTimeZoneRemoved, this, &Datetime::removeTimezone);
    connect(model, &DatetimeModel::NTPChanged, m_ntpSwitch, &SwitchWidget::setChecked);

    addTimezones(model->userTimeZones());
    m_ntpSwitch->setChecked(model->nTP());
}

//void Datetime::addTimezone(const Timezone &tz)
//{
//    if(m_addeds.contains(tz))
//    {
//        return;
//    }
//    TimezoneItem* item = new TimezoneItem();
//    connect(m_headItem, SIGNAL(editChanged(bool)), item, SLOT(slotStatus(bool)));
//    connect(item, SIGNAL(destroySelf()), m_headItem, SLOT(initStatus()));
//    connect(item, SIGNAL(removeTimezone(Timezone)), this, SLOT(slotRemoveTimezone(Timezone)));

//    item->setCity(tz);
//    item->slotStatus(m_bEdit);
//    m_group->appendItem(item);
//    m_addeds.append(tz);

//    m_settings->beginGroup(tz.m_city);
//    m_settings->setValue("City", tz.m_city);
//    m_settings->setValue("Timezone", tz.m_timezone);
//    m_settings->endGroup();
//}

void Datetime::slotClick()
{
}

void Datetime::slotEditMode(bool edit)
{
    m_bEdit = edit;
//    m_addItem->setEnabled(!m_bEdit);
}

//void Datetime::slotRemoveTimezone(const Timezone &tz)
//{
//    TimezoneItem* item = qobject_cast<TimezoneItem*>(sender());
//    if(item)
//    {
//        m_timezoneGroup->removeItem(item);
//        item->deleteLater();
//        m_settings->remove(tz.m_city+"/"+"City");
//        m_settings->remove(tz.m_city+"/"+"Timezone");
//    }
//}

void Datetime::addTimezone(const ZoneInfo &zone)
{
    qDebug() << "user time zone added: " << zone;

    TimezoneItem* item = new TimezoneItem;

//    connect(m_headItem, SIGNAL(editChanged(bool)), item, SLOT(slotStatus(bool)));
//    connect(item, SIGNAL(destroySelf()), m_headItem, SLOT(initStatus()));
//    connect(item, SIGNAL(removeTimezone(Timezone)), this, SLOT(slotRemoveTimezone(Timezone)));

    item->setTimeZone(zone);
    item->slotStatus(m_bEdit);

    m_timezoneGroup->appendItem(item);
}

void Datetime::addTimezones(const QList<ZoneInfo> &zones)
{
    qDebug() << "add user timezones: " << zones;

    for (const ZoneInfo &zone : zones) {
        addTimezone(zone);
    }
}

void Datetime::removeTimezone(const ZoneInfo &zone)
{
    QList<TimezoneItem*> items = findChildren<TimezoneItem*>();
    for (TimezoneItem *item : items) {
        if (item->timeZone().getZoneName() == zone.getZoneName()) {
            item->setVisible(false);
            m_timezoneGroup->removeItem(item);
            item->deleteLater();
        }
    }
}

}
}
