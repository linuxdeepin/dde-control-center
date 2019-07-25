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
#include "modules/datetime/timezone_dialog/timezonechooser.h"
#include "timesetting.h"
#include "datesettings.h"
#include "modules/datetime/datesettings.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::datetime;

DatetimeModule::DatetimeModule(FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_mainWidget(nullptr)
{

}

void DatetimeModule::initialize()
{
    m_mainWidget = new DatetimeWidget;

    m_model = new dcc::datetime::DatetimeModel;
    m_work = new dcc::datetime::DatetimeWork(m_model);

    QThread::currentThread()->msleep(500);

    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());

    connect(m_mainWidget, &DatetimeWidget::requestPushWidget, this, &DatetimeModule::onPushWidget);
}

void DatetimeModule::reset()
{

}

const QString DatetimeModule::name() const
{
    return QStringLiteral("datetime");
}

void DatetimeModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName)
}

QWidget *DatetimeModule::moduleWidget()
{
    m_mainWidget->setModel(m_model);
    m_work->activate(); //refresh data

    return m_mainWidget;
}

void DatetimeModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

void DatetimeModule::createWidget(int index)
{

}

void DatetimeModule::showTimezoneList()
{
    TimezoneList *timezonelist = new TimezoneList;

    m_frameProxy->pushWidget(this, timezonelist);
}

void DatetimeModule::showSystemTimezone()
{
    //only pop third-level page
}

void DatetimeModule::showTimeSetting()
{
    DateSettings *setting = new DateSettings;

    connect(setting, &DateSettings::requestSetAutoSyncdate, m_work, &dcc::datetime::DatetimeWork::setNTP);
    connect(setting, &DateSettings::requestSetTime, m_work, &dcc::datetime::DatetimeWork::setDatetime);
    connect(setting, &DateSettings::requestBack, this, &DatetimeModule::onPopWidget);
    connect(m_mainWidget, &DatetimeWidget::requestSetNtp, setting, &DateSettings::updateRealAutoSyncCheckState);
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
