#include "datetime.h"

#include <QFrame>
#include <QDebug>
#include <QPushButton>

#include "datetimemodel.h"
#include "clockitem.h"
#include "timezoneitem.h"
#include "timezone_dialog/timezonechooser.h"

namespace dcc {
namespace datetime {

Datetime::Datetime()
    : ModuleWidget(),
      m_model(nullptr),
      m_timeSettingsGroup(new SettingsGroup),
      m_ntpSwitch(new SwitchWidget(tr("Auto-Sync"))),
      m_timePageButton(new NextPageWidget),
      m_timezoneGroup(new SettingsGroup),
      m_headItem(new SettingsHead),
      m_addTimezoneButton(new QPushButton(tr("Add Timezone"))),
      m_dialog(new TimeZoneChooser)
{
    setObjectName("Datetime");
    setTitle(tr("Time and Date"));

    this->installEventFilter(parent());

    SettingsGroup *clockGroup = new SettingsGroup;
    ClockItem *clock = new ClockItem;
    clockGroup->appendItem(clock);

    m_timePageButton->setTitle(tr("Time Settings"));
    m_timeSettingsGroup->appendItem(m_ntpSwitch);
    m_timeSettingsGroup->appendItem(m_timePageButton);

    m_headItem->setTitle(tr("Timezone List"));

    m_timezoneGroup->appendItem(m_headItem);

    m_dialog->setFixedSize(1366, 768);

    m_centeralLayout->addWidget(clockGroup);
    m_centeralLayout->addSpacing(10);
    m_centeralLayout->addWidget(m_timeSettingsGroup);
    m_centeralLayout->addSpacing(10);
    m_centeralLayout->addWidget(m_timezoneGroup);
    m_centeralLayout->addSpacing(10);
    m_centeralLayout->addWidget(m_addTimezoneButton);

    connect(m_dialog, &TimeZoneChooser::confirmed, this, &Datetime::requestAddUserTimeZone);

    connect(m_ntpSwitch, &SwitchWidget::checkedChanegd, this, &Datetime::requestSetNtp);
    connect(m_timePageButton, &NextPageWidget::clicked, this, &Datetime::requestTimeSettings);
    connect(m_addTimezoneButton, &QPushButton::clicked, m_dialog, &TimeZoneChooser::show);

    connect(m_headItem, &SettingsHead::editChanged, this, &Datetime::onEditClicked);
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

void Datetime::addTimezone(const ZoneInfo &zone)
{
    qDebug() << "user time zone added: " << zone;

    if (zone.getZoneName() == QTimeZone::systemTimeZoneId()) return;

    TimezoneItem* item = new TimezoneItem;

    connect(item, &TimezoneItem::removeClicked, [this, item] {
        item->setVisible(false);
        m_headItem->setEditEnable(false);
        emit requestRemoveUserTimeZone(item->timeZone());
    });

    item->setTimeZone(zone);

    m_timezoneGroup->appendItem(item);

    m_headItem->setEditEnable(true);
}

void Datetime::addTimezones(const QList<ZoneInfo> &zones)
{
    qDebug() << "add user timezones: " << zones;

    m_headItem->setEditEnable(false);

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
        } else {
            item->toNormalMode();
        }
    }

    m_headItem->blockSignals(true);
    m_headItem->initStatus();
    m_headItem->blockSignals(false);

    m_headItem->setEditEnable(items.length() > 1);
}

void Datetime::onEditClicked(const bool &edit)
{
    QList<TimezoneItem*> items = findChildren<TimezoneItem*>();
    for (TimezoneItem *item : items) {
        if (edit) {
            item->toRemoveMode();
        } else {
            item->toNormalMode();
        }
    }
}

}
}
