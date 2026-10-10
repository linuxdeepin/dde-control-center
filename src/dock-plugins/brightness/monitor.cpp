// SPDX-FileCopyrightText: 2011 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "monitor.h"
#include "brightnesscontroller.h"

#include <QGuiApplication>

const double DoubleZero = 0.000001;

Monitor::Monitor(QObject *parent)
    : QObject(parent)
    , m_brightness(1.0)
    , m_enable(false)
    , m_canBrightness(true)
{
}

void Monitor::setPrimary(const QString &primaryName)
{
    m_primary = primaryName;
}

void Monitor::setBrightness(const double brightness)
{
    if (fabs(m_brightness - brightness) < DoubleZero)
        return;

    m_brightness = brightness;

    Q_EMIT brightnessChanged(m_brightness);
}

void Monitor::setName(const QString &name)
{
    m_name = name;
}

void Monitor::setCanBrightness(bool canBrightness)
{
    m_canBrightness = canBrightness;
}

void Monitor::setPath(const QString &path)
{
    m_path = path;
}

void Monitor::setBuildin(bool buildin)
{
    m_buildin = buildin;
}

void Monitor::setMonitorEnable(bool enable)
{
    if (m_enable == enable)
        return;

    m_enable = enable;
    Q_EMIT enableChanged(enable);
}
