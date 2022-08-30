// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "timezonecontentlist.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingsitem.h"
#include "modules/datetime/timezoneitem.h"
#include "modules/datetime/timezone_dialog/timezone.h"
#include "modules/datetime/timezone_dialog/timezonechooser.h"

using namespace dcc;
using namespace dcc::datetime;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::datetime;

TimezoneContentList::TimezoneContentList(QWidget *parent)
    : ContentWidget(parent)
    , m_centralLayout(new QVBoxLayout)
    , m_timezoneGroup(new SettingsGroup)
{
    setAccessibleName("TimezoneContentList");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(0);
    m_centralLayout->addWidget(m_timezoneGroup, 0, Qt::AlignTop);
    m_centralLayout->addStretch();

    QWidget *mainWidget = new TranslucentFrame(this);
    mainWidget->setLayout(m_centralLayout);
    layout()->setMargin(0);
    setContent(mainWidget);
}

TimezoneContentList::~TimezoneContentList()
{
    m_timezoneGroup->clear();

    for (auto it = m_zoneList.begin(); it != m_zoneList.end();) {
        TimezoneItem *item = (*it);
        it = m_zoneList.erase(it);//equal ++it
        item->deleteLater();
    }

    m_zoneList.clear();
}

void TimezoneContentList::setSpace(int value)
{
    if (m_timezoneGroup) {
        m_timezoneGroup->setSpacing(value - 2);
    }
}

void TimezoneContentList::updateTimezoneItems()
{
    QList<TimezoneItem *>::Iterator iter = m_zoneList.begin();
    for (; iter != m_zoneList.end(); iter++) {
        (*iter)->updateInfo();
    }
}

void TimezoneContentList::onEditClicked(const bool &edit)
{
    QList<TimezoneItem *>::Iterator iter = m_zoneList.begin();
    for (; iter != m_zoneList.end(); iter++) {
        if (edit) {
            (*iter)->toRemoveMode();
        } else {
            (*iter)->toNormalMode();
        }
    }
}

void TimezoneContentList::addTimezone(const ZoneInfo &zone)
{
//    qDebug() << "user time zone added: " << zone;

    TimezoneItem *item = new TimezoneItem;

    connect(item, &TimezoneItem::removeClicked, [this, item] {
        item->setVisible(false);
        Q_EMIT requestRemoveUserTimeZone(item->timeZone());
    });

    item->setTimeZone(zone);

    if (zone.getZoneName() != QTimeZone::systemTimeZoneId()) {
        m_timezoneGroup->appendItem(item);
    }

    m_zoneList << item;

    Q_EMIT notifyItemCount(m_zoneList.count());
}

void TimezoneContentList::addTimezones(const QList<ZoneInfo> &zones)
{
    qDebug() << "add user timezones: " << zones;

    for (const ZoneInfo &zone : zones) {
        addTimezone(zone);
    }

    if (zones.count() == 0)
        Q_EMIT notifyItemCount(m_zoneList.count());
}

void TimezoneContentList::removeTimezone(const ZoneInfo &zone)
{
    for (auto it = m_zoneList.begin(); it != m_zoneList.end();) {
        TimezoneItem *item = (*it);
        if (item->timeZone().getZoneName() == zone.getZoneName()) {
            item->setVisible(false);
            m_timezoneGroup->removeItem(item);
            it = m_zoneList.erase(it);
            item->deleteLater();
        } else {
            item->toNormalMode();
            ++it;
        }
    }

    Q_EMIT notifyItemCount(m_zoneList.count());
}

void TimezoneContentList::updateTimezones(const QList<ZoneInfo> &zones)
{
    m_timezoneGroup->clear();

    for (auto it = m_zoneList.begin(); it != m_zoneList.end();) {
        TimezoneItem *item = (*it);
        it = m_zoneList.erase(it);//equal ++it
        item->deleteLater();
    }

    m_zoneList.clear();

    addTimezones(zones);
}
