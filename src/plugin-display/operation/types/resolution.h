//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
