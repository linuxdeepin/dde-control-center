/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
      m_addTimeZone(false)
{
    setObjectName("Datetime");
    setTitle(tr("Time and Date"));

    this->installEventFilter(parent());
#ifndef DCC_DISABLE_TIMEZONE
    m_timezoneItem->setTitle(tr("Change System Timezone"));
#endif
    SettingsGroup *clockGroup = new SettingsGroup;
    m_clockItem = new ClockItem;
    clockGroup->appendItem(m_clockItem);
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
    connect(m_addTimezoneButton, &QPushButton::clicked, this, [this] {
        if (m_dialog && m_dialog->isVisible()) return;

        Q_EMIT requestHold();

        ensureZoneChooserDialog();

        m_dialog->setIsAddZone(true);
        m_dialog->show();
    }, Qt::QueuedConnection);

    connect(m_timezoneItem, &NextPageWidget::clicked, this, [this] {
        if (m_dialog && m_dialog->isVisible()) return;

        Q_EMIT requestHold();

        ensureZoneChooserDialog();

        m_dialog->setIsAddZone(false);
        m_dialog->show();
        m_dialog->setMarkedTimeZone(installer::GetCurrentTimezone());
    }, Qt::QueuedConnection);

    connect(m_headItem, &SettingsHead::editChanged, this, &Datetime::onEditClicked);
#endif
}

Datetime::~Datetime()
{
    if (m_dialog)
        m_dialog->deleteLater();
}

void Datetime::setModel(const DatetimeModel *model)
{
    m_model = model;


#ifndef DCC_DISABLE_TIMEZONE
    connect(model, &DatetimeModel::userTimeZoneAdded, this, &Datetime::addTimezone);
    connect(model, &DatetimeModel::userTimeZoneRemoved, this, &Datetime::removeTimezone);

#else
    connect(model, &DatetimeModel::userTimeZoneAdded, m_clockItem, &ClockItem::setTimeZone);
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

    m_clockItem->setTimeZone(m_model->currentTimeZone());
}

#ifndef DCC_DISABLE_TIMEZONE
void Datetime::addTimezone(const ZoneInfo &zone)
{
    qDebug() << "user time zone added: " << zone;

    TimezoneItem* item = new TimezoneItem;

    connect(item, &TimezoneItem::removeClicked, [this, item] {
        item->setVisible(false);
        m_headItem->setEditEnable(false);
        Q_EMIT requestRemoveUserTimeZone(item->timeZone());
    });

    item->setTimeZone(zone);

    if (zone.getZoneName() != QTimeZone::systemTimeZoneId()) {
        m_timezoneGroup->appendItem(item);
    } else {
        m_clockItem->setTimeZone(zone);
    }

    m_zoneList << item;

    m_headItem->setVisible(m_zoneList.size() >= 2);
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
            m_zoneList.removeOne(item);
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

    if (m_dialog)
        m_dialog->close();

    m_timezoneGroup->clear();
    m_zoneList.clear();

    m_timezoneGroup->appendItem(m_headItem);

    addTimezones(m_model->userTimeZones());
}

void Datetime::onEditClicked(const bool &edit)
{
    for (TimezoneItem *item : m_zoneList) {
        if (edit) {
            item->toRemoveMode();
        } else {
            item->toNormalMode();
        }
    }
}

void Datetime::ensureZoneChooserDialog()
{
    if (m_dialog)
        return;

    m_dialog = new TimeZoneChooser();
    m_dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(m_dialog, &TimeZoneChooser::confirmed, this, [this] (const QString &timezone) {
        if (m_dialog->isAddZone()) {
            Q_EMIT requestAddUserTimeZone(timezone);
        } else {
            Q_EMIT requestSetTimeZone(timezone);
        }

        m_dialog->close();
        Q_EMIT requestUnhold();
    });
    connect(m_dialog, &TimeZoneChooser::cancelled, this, &Datetime::requestUnhold);
}
#endif

}
}
