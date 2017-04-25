#include "datetime.h"

#include <QFrame>
#include <QDebug>
#include <QPushButton>

#include "datetimemodel.h"
#include "clockitem.h"
#include "timezoneitem.h"

#include "timezone_dialog/timezone.h"
#include "timezone_dialog/timezonechooser.h"

namespace dcc {
namespace datetime {

Datetime::Datetime()
    : ModuleWidget(),
      m_model(nullptr),
      m_timeSettingsGroup(new SettingsGroup),
      m_ntpSwitch(new SwitchWidget(tr("Auto-Sync"))),
      m_timePageButton(new NextPageWidget),
#ifndef DCC_DISABLE_TIMEZONE
      m_timezoneGroup(new SettingsGroup),
      m_headItem(new SettingsHead),
      m_addTimezoneButton(new QPushButton(tr("Add Timezone"))),
      m_timezoneItem(new NextPageWidget),
#endif
      m_addTimeZone(false),
      m_dialog(new TimeZoneChooser)
{
    setObjectName("Datetime");
    setTitle(tr("Time and Date"));

    this->installEventFilter(parent());
#ifndef DCC_DISABLE_TIMEZONE
    m_timezoneItem->setTitle(tr("Change System Timezone"));
#endif
    SettingsGroup *clockGroup = new SettingsGroup;
    ClockItem *clock = new ClockItem;
    clockGroup->appendItem(clock);
#ifndef DCC_DISABLE_TIMEZONE
    clockGroup->appendItem(m_timezoneItem);
#endif
    m_timePageButton->setTitle(tr("Time Settings"));
    m_timeSettingsGroup->appendItem(m_ntpSwitch);
    m_timeSettingsGroup->appendItem(m_timePageButton);

#ifndef DCC_DISABLE_TIMEZONE
    m_headItem->setTitle(tr("Timezone List"));
    m_headItem->setVisible(false);
    m_timezoneGroup->appendItem(m_headItem);
#endif

    m_centralLayout->addWidget(clockGroup);
    m_centralLayout->addWidget(m_timeSettingsGroup);
#ifndef DCC_DISABLE_TIMEZONE
    m_centralLayout->addWidget(m_timezoneGroup);
    m_centralLayout->addWidget(m_addTimezoneButton);
#endif

    connect(m_ntpSwitch, &SwitchWidget::checkedChanged, this, &Datetime::requestSetNtp);
    connect(m_timePageButton, &NextPageWidget::clicked, this, &Datetime::requestTimeSettings);

#ifndef DCC_DISABLE_TIMEZONE
    connect(m_dialog, &TimeZoneChooser::confirmed, this, [this] (const QString &timezone) {
        if (m_addTimeZone) {
            emit requestAddUserTimeZone(timezone);
        } else {
            emit requestSetTimeZone(timezone);
        }

        emit requestUnhold();
    });
    connect(m_dialog, &TimeZoneChooser::cancelled, this, [this] {
        emit requestUnhold();
    });


    connect(m_addTimezoneButton, &QPushButton::clicked, this, [this] {
        m_addTimeZone = true;

        emit requestHold();
        m_dialog->show();
    });

    connect(m_timezoneItem, &NextPageWidget::clicked, this, [this] {
        m_addTimeZone = false;

        emit requestHold();
        m_dialog->show();
    });

    connect(m_headItem, &SettingsHead::editChanged, this, &Datetime::onEditClicked);
#endif
}

Datetime::~Datetime()
{
}

void Datetime::setModel(const DatetimeModel *model)
{
    m_model = model;

#ifndef DCC_DISABLE_TIMEZONE
    connect(model, &DatetimeModel::userTimeZoneAdded, this, &Datetime::addTimezone);
    connect(model, &DatetimeModel::userTimeZoneRemoved, this, &Datetime::removeTimezone);
#endif

    // we need to update all the timezone items after the system time has changed.
    connect(model, &DatetimeModel::NTPChanged, [this] (const bool &ntp){
        m_ntpSwitch->setChecked(ntp);
        updateTimezoneItems();
    });
    connect(model, &DatetimeModel::systemTimeChanged, [this] {
        updateTimezoneItems();
    });

#ifndef DCC_DISABLE_TIMEZONE
    connect(model, &DatetimeModel::systemTimeZoneIdChanged,
            this, &Datetime::updateSystemTimezone);

    addTimezones(model->userTimeZones());
    updateSystemTimezone(model->systemTimeZoneId());
#endif
    m_ntpSwitch->setChecked(model->nTP());
}

#ifndef DCC_DISABLE_TIMEZONE
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

    m_headItem->setVisible(true);
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

    if (items.length() == 1)
        m_headItem->setVisible(false);

//    m_headItem->blockSignals(true);
//    m_headItem->initStatus();
//    m_headItem->blockSignals(false);

    m_headItem->setEditEnable(items.length() > 1);
}
#endif

void Datetime::updateTimezoneItems()
{
    QList<TimezoneItem*> items = findChildren<TimezoneItem*>();
    for (TimezoneItem *item : items) {
        item->updateInfo();
    }
}

#ifndef DCC_DISABLE_TIMEZONE
void Datetime::updateSystemTimezone(const QString &timezone)
{
    if (timezone.isEmpty()) return;

    const QString locale = QLocale::system().name();
    const QString name = installer::GetLocalTimezoneName(timezone, locale);
    m_timezoneItem->setValue(name);
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
#endif

}
}
