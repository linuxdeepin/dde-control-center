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

#ifndef WIRELESSDEVICE_H
#define WIRELESSDEVICE_H

#include "networkdevice.h"

#include <QMap>

namespace dcc {

namespace network {

class WirelessDevice : public NetworkDevice
{
    Q_OBJECT

public:
    explicit WirelessDevice(const QJsonObject &info, QObject *parent = 0);

    bool supportHotspot() const;
    const QString hotspotUuid() const;
    inline bool hotspotEnabled() const { return supportHotspot() && !m_hotspotInfo.isEmpty(); }
    inline const QString activeApName() const { return m_activeAp; }

signals:
    void apAdded(const QJsonObject &apInfo) const;
    void apInfoChanged(const QJsonObject &apInfo) const;
    void apRemoved(const QString &ssid) const;
    void activeApChanged(const QString &oldName, const QString &newName) const;
    void hotspotEnabledChanged(const bool enabled) const;

public slots:
    void setAPList(const QString &apList);
    void updateAPInfo(const QString &apInfo);
    void deleteAP(const QString &apInfo);
    void setActiveApName(const QString &name);
    void setHotspotInfo(const QJsonObject &hotspotInfo);

private:
    QSet<QString> m_aps;
    QString m_activeAp;
    QJsonObject m_hotspotInfo;
};

}

}

#endif // WIRELESSDEVICE_H
