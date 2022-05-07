/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <QDBusMetaType>

class Resolution
{
public:
    friend QDebug operator<<(QDebug debug, const Resolution &resolution);
    friend QDBusArgument &operator<<(QDBusArgument &arg, const Resolution &value);
    friend const QDBusArgument &operator>>(const QDBusArgument &arg, Resolution &value);

    explicit Resolution();

    bool operator!=(const Resolution &other) const;
    bool operator==(const Resolution &other) const;

    quint32 id() const { return m_id; }
    quint16 width() const { return m_width; }
    quint16 height() const { return m_height; }
    double rate() const { return m_rate; }

public:
    quint32 m_id;
    quint16 m_width;
    quint16 m_height;
    double m_rate;
};


Q_DECLARE_METATYPE(Resolution)

void registerResolutionMetaType();

#endif // RESOLUTION_H
