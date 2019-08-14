/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#include "datetimemodule.h"
#include "datetimewidget.h"
#include "modules/datetime/datetimework.h"
#include "modules/datetime/datetimemodel.h"
#include "timezonelist.h"
#include "modules/datetime/timezone_dialog/timezone.h"
#include "modules/datetime/timezone_dialog/timezonechooser.h"
#include "datesettings.h"
#include "modules/datetime/datesettings.h"

using namespace dcc::datetime;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::datetime;

DatetimeModule::DatetimeModule(FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_timezonelist(nullptr)
{

}

void DatetimeModule::initialize()
{
    m_model = new dcc::datetime::DatetimeModel;
    m_work = new dcc::datetime::DatetimeWork(m_model);

    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());

#ifndef DCC_DISABLE_TIMEZONE
    connect(this, &DatetimeModule::requestSetTimeZone, m_work, &DatetimeWork::setTimezone);
    connect(this, &DatetimeModule::requestRemoveUserTimeZone, m_work, &DatetimeWork::removeUserTimeZone);
    connect(this, &DatetimeModule::requestAddUserTimeZone, m_work, &DatetimeWork::addUserTimeZone);
#endif
}

const QString DatetimeModule::name() const
{
    return QStringLiteral("datetime");
}

void DatetimeModule::active()
{
    DatetimeWidget* widget = new DatetimeWidget;
    connect(widget, &DatetimeWidget::requestPushWidget, this, &DatetimeModule::onPushWidget);
    widget->setModel(m_model);
    m_work->activate(); //refresh data

    m_frameProxy->pushWidget(this, widget);
}

void DatetimeModule::createWidget(int index)
{
    Q_UNUSED(index);
}

void DatetimeModule::updateSystemTimezone(const QString &timezone)
{
    if (timezone.isEmpty()) return;

    const QString locale = QLocale::system().name();
    const QString name = installer::GetLocalTimezoneName(timezone, locale);

    if (m_dialog) {
        m_dialog->setCurrentTimeZoneText(name);
        m_dialog->close();
    }

    if (m_timezonelist && m_model) {
        m_timezonelist->getTimezoneContentListPtr()->updateTimezones(m_model->userTimeZones());
    }
}

void DatetimeModule::ensureZoneChooserDialog()
{
    if (m_dialog)
        return;

    m_dialog = new TimeZoneChooser();
    m_dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(m_dialog, &TimeZoneChooser::confirmed, this, [this](const QString & timezone) {
        if (m_dialog->isAddZone()) {
            Q_EMIT requestAddUserTimeZone(timezone);
        } else {
            Q_EMIT requestSetTimeZone(timezone);
        }

        m_dialog->close();
    });

    connect(m_model, &dcc::datetime::DatetimeModel::systemTimeZoneIdChanged, this, &DatetimeModule::updateSystemTimezone);
}

void DatetimeModule::showTimezoneList()
{
    if (!m_timezonelist) {
        m_timezonelist = new TimezoneList;

        //first into this page, update timezonelist data
        m_timezonelist->getTimezoneContentListPtr()->addTimezones(m_model->userTimeZones());
        updateSystemTimezone(m_model->systemTimeZoneId());

        connect(m_model, &DatetimeModel::userTimeZoneAdded,
                m_timezonelist->getTimezoneContentListPtr(), &TimezoneContentList::addTimezone);
        connect(m_model, &DatetimeModel::userTimeZoneRemoved,
                m_timezonelist->getTimezoneContentListPtr(), &TimezoneContentList::removeTimezone);
        // we need to update all the timezone items after the system time has changed.
        connect(m_model, &DatetimeModel::NTPChanged,
                m_timezonelist->getTimezoneContentListPtr(), &TimezoneContentList::updateTimezoneItems);
        connect(m_model, &DatetimeModel::systemTimeChanged,
                m_timezonelist->getTimezoneContentListPtr(), &TimezoneContentList::updateTimezoneItems);
        connect(m_timezonelist, &TimezoneList::requestAddTimeZone, this, [this] {
            ensureZoneChooserDialog();
            m_dialog->setIsAddZone(true);
            m_dialog->show();
        });
        connect(m_timezonelist->getTimezoneContentListPtr(),&TimezoneContentList::requestRemoveUserTimeZone,
                this, &DatetimeModule::requestRemoveUserTimeZone);
        connect(m_timezonelist, &TimezoneList::requestAddUserTimeZone,
                this, &DatetimeModule::requestAddUserTimeZone);

        m_frameProxy->pushWidget(this, m_timezonelist);
    }
}

void DatetimeModule::showSystemTimezone()
{
    if (m_dialog && m_dialog->isVisible()) return;

    ensureZoneChooserDialog();
    m_dialog->setIsAddZone(false);
    m_dialog->show();
    m_dialog->setMarkedTimeZone(installer::GetCurrentTimezone());
}

void DatetimeModule::showTimeSetting()
{
    DateSettings *setting = new DateSettings;

    connect(setting, &DateSettings::requestSetAutoSyncdate, m_work, &dcc::datetime::DatetimeWork::setNTP);
    connect(setting, &DateSettings::requestSetTime, m_work, &dcc::datetime::DatetimeWork::setDatetime);
    connect(setting, &DateSettings::requestBack, this, &DatetimeModule::onPopWidget);
    connect(m_model, &dcc::datetime::DatetimeModel::NTPChanged, setting, &DateSettings::updateRealAutoSyncCheckState);

    setting->updateRealAutoSyncCheckState(m_model->nTP());

    m_frameProxy->pushWidget(this, setting);
}

void DatetimeModule::onPushWidget(const int &index)
{
    switch (static_cast<datetimeType>(index)) {
    case TIMEZONELIST:
        showTimezoneList();
        break;
    case SYSTEMTIMEZONE:
        showSystemTimezone();
        break;
    case TIMESETTING:
        showTimeSetting();
        break;
    default:
        break;
    }
}

void DatetimeModule::onPopWidget()
{
    m_frameProxy->popWidget(this);
}
