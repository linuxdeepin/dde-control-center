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

#ifndef MIRACASTDEVICEMODEL_H
#define MIRACASTDEVICEMODEL_H

#include "miracastitem.h"
#include <types/sinkinfolist.h>
#include "types/linkinfolist.h"
#include <QObject>

namespace dcc{
namespace display {
class MiracastDeviceModel : public QObject
{
    Q_OBJECT
public:
    explicit MiracastDeviceModel(const LinkInfo &linkinfo, QObject *parent = 0);
    ~MiracastDeviceModel();

    const QList<SinkInfo> sinkList() const;
    const LinkInfo linkInfo() const;
    bool isConnected() const;

Q_SIGNALS:
    void addSink(const SinkInfo &info);
    void removeSink(const SinkInfo &info);
    void linkManageChanged(const bool state);
    void connectStateChanged(const QDBusObjectPath &sinkPath, const bool state);

public Q_SLOTS:
    void onSinkAdded(const SinkInfo &sinkinfo);
    void onSinkRemoved(const SinkInfo &sinkinfo);
    void onSinkConnect(const QDBusObjectPath &sinkPath, bool connected);
    void onLinkManageChanged(const bool state);

private:
    QList<SinkInfo> m_sinkList;
    LinkInfo m_linkInfo;
};
}
}

#endif // MIRACASTDEVICEMODEL_H
