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

#include "datetimemodule.h"

namespace dcc {
namespace datetime {

DatetimeModule::DatetimeModule(FrameProxyInterface *frame, QObject *parent)
    :QObject(parent),
      ModuleInterface(frame),
      m_datetimeWidget(nullptr),
      m_dateSettings(nullptr)
{
}

void DatetimeModule::initialize()
{
    // NOTE: sbw: delay
    // see: https://tower.im/projects/1c0cd0c59ef941298c6e6b2ba6833b91/todos/8968883fa139470183517581fada79ee/
    QThread::currentThread()->msleep(500);

    m_model = new DatetimeModel;
    m_work = new DatetimeWork(m_model);

    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());

    connect(m_work, &DatetimeWork::requestSetAutoHide, this, &DatetimeModule::setFrameAutoHide);
}

void DatetimeModule::moduleActive()
{
    m_work->activate();
}

void DatetimeModule::moduleDeactive()
{
    m_work->deactivate();
}

void DatetimeModule::reset()
{

}

ModuleWidget *DatetimeModule::moduleWidget()
{
    if(!m_datetimeWidget)
    {
        m_datetimeWidget = new Datetime();
        m_datetimeWidget->setModel(m_model);

        connect(m_datetimeWidget, &Datetime::requestSetNtp, m_work, &DatetimeWork::setNTP);
        connect(m_datetimeWidget, &Datetime::requestTimeSettings, this, &DatetimeModule::showTimeSettingsPage);
#ifndef DCC_DISABLE_TIMEZONE
        connect(m_datetimeWidget, &Datetime::requestRemoveUserTimeZone, m_work, &DatetimeWork::removeUserTimeZone);
        connect(m_datetimeWidget, &Datetime::requestAddUserTimeZone, m_work, &DatetimeWork::addUserTimeZone);
        connect(m_datetimeWidget, &Datetime::requestSetTimeZone, m_work, &DatetimeWork::setTimezone);

        connect(m_datetimeWidget, &Datetime::requestHold, this, [this] {
            m_frameProxy->setFrameAutoHide(this, false);
        }, Qt::QueuedConnection);
        connect(m_datetimeWidget, &Datetime::requestUnhold, this, [this] {
            m_frameProxy->setFrameAutoHide(this, true);
        }, Qt::QueuedConnection);
#endif
    }

    return m_datetimeWidget;
}

const QString DatetimeModule::name() const
{
    return QStringLiteral("datetime");
}

void DatetimeModule::contentPopped(ContentWidget * const w)
{
    if (w == m_dateSettings)
        m_dateSettings = nullptr;

    w->deleteLater();
}

void DatetimeModule::showTimeSettingsPage()
{
    if (!m_dateSettings) {
        m_dateSettings = new DateSettings;
        m_dateSettings->setCurrentTimeZone(m_model->currentTimeZone());

        connect(m_dateSettings, &DateSettings::requestSetTime, m_work, &DatetimeWork::setDatetime);
    }

    m_frameProxy->pushWidget(this, m_dateSettings);
}

void DatetimeModule::setFrameAutoHide(const bool visible)
{
    m_frameProxy->setFrameAutoHide(this, visible);
}

DatetimeModule::~DatetimeModule()
{
    m_work->deleteLater();
    m_model->deleteLater();
}

}
}
