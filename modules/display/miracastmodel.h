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

#ifndef MIRACASTMODEL_H
#define MIRACASTMODEL_H

#include "miracastworker.h"
#include "miracastdevicemodel.h"
#include <QObject>

namespace dcc {
namespace display {
class MiracastModel : public QObject
{
    Q_OBJECT

    friend class MiracastWorker;

public:
    enum EventType
    {
        LinkManaged = 1,
        LinkUnmanaged,
        SinkConnected,
        SinkConnectFailed,
        SinkDisconnected,
    };

    explicit MiracastModel(QObject *parent = 0);

    const QList<LinkInfo> links() const { return m_links; }

    MiracastDeviceModel* deviceModelByPath(const QString &path);
    LinkInfo &linkByPath(const QDBusObjectPath &path);

Q_SIGNALS:
    void linkAdded(const LinkInfo &link) const;
    void linkRemoved(const QDBusObjectPath &path) const;
    void requestLinkScanning(const QDBusObjectPath &path, const bool scanning) const;
    void linkEnableChanged(const QDBusObjectPath &path, const bool enable) const;
    void sinkConnected() const;

private:
    void addSink(const SinkInfo &peer);
    void addLink(const LinkInfo &link);
    void removeLink(const QDBusObjectPath &path);
    void removeSink(const QString &peerInfo);
    void setLinks(const QList<LinkInfo> &links);
    void setSinks(const QList<SinkInfo> &sinks);
    void onPathAdded(const QDBusObjectPath &path, const QString &info);
    void onPathRemoved(const QDBusObjectPath &path, const QString &info);
    void onMiracastEvent(const uchar type, const QDBusObjectPath &path);
    void scanAllLinks();
    void sinkStateChanged(const QDBusObjectPath &path, bool state);

private:
    QList<LinkInfo> m_links;
    QList<SinkInfo> m_sinks;
    QMap<QString, MiracastDeviceModel*> m_deviceModelList;
};
}
}

#endif // MIRACASTMODEL_H
