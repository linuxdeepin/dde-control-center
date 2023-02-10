//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "resolution.h"

#include <QDebug>

void registerResolutionMetaType()
{
    qRegisterMetaType<Resolution>("Resolution");
    qDBusRegisterMetaType<Resolution>();
}

QDebug operator<<(QDebug debug, const Resolution &resolution)
{
    debug << QString("Resolution(%1, %2, %3, %4)").arg(resolution.m_id)
                                                    .arg(resolution.m_width)
                                                    .arg(resolution.m_height)
                                                    .arg(resolution.m_rate);

    return debug;
}

Resolution::Resolution()
    : m_rate(0.0)
{
}

bool Resolution::operator!=(const Resolution &other) const
{
    return m_width != other.m_width || m_height != other.m_height || m_rate != other.m_rate;
}

bool Resolution::operator==(const Resolution &other) const
{
    return !(other != *this);
}

QDBusArgument &operator<<(QDBusArgument &arg, const Resolution &value)
{
    arg.beginStructure();
    arg << value.m_id << value.m_width << value.m_height << value.m_rate;
    arg.endStructure();

    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, Resolution &value)
{
    arg.beginStructure();
    arg >> value.m_id >> value.m_width >> value.m_height >> value.m_rate;
    arg.endStructure();
    return arg;
}
