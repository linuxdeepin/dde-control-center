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

#include "miracastdevicemodel.h"
#include <QDebug>

using namespace dcc;
using namespace dcc::display;

MiracastDeviceModel::MiracastDeviceModel(const LinkInfo &linkInfo, QObject *parent) :
    QObject(parent),
    m_linkInfo(linkInfo)
{

}

MiracastDeviceModel::~MiracastDeviceModel()
{
    m_sinkList.clear();
}

const QList<SinkInfo> MiracastDeviceModel::sinkList() const
{
    return m_sinkList;
}

const LinkInfo MiracastDeviceModel::linkInfo() const
{
    return m_linkInfo;
}

bool MiracastDeviceModel::isConnected() const
{
    for (const SinkInfo &info : m_sinkList)
        if (info.m_connected)
            return true;

    return false;
}

void dcc::display::MiracastDeviceModel::onSinkAdded(const SinkInfo &sinkinfo)
{
    if (m_sinkList.contains(sinkinfo))
        return;

    m_sinkList.append(sinkinfo);

    Q_EMIT addSink(sinkinfo);
}

void dcc::display::MiracastDeviceModel::onSinkRemoved(const SinkInfo &sinkinfo)
{
    m_sinkList.removeOne(sinkinfo);
    Q_EMIT removeSink(sinkinfo);
}

void MiracastDeviceModel::onSinkConnect(const QDBusObjectPath &sinkPath, bool connected)
{
    for (SinkInfo &info : m_sinkList)
        if (info.m_sinkPath == sinkPath)
            info.m_connected = connected;

    Q_EMIT connectStateChanged(sinkPath, connected);
}

void MiracastDeviceModel::onLinkManageChanged(const bool state)
{
    if (m_linkInfo.m_managed == state)
        return;

    m_linkInfo.m_managed = state;
    Q_EMIT linkManageChanged(state);
}
