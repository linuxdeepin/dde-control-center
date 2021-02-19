/*
 * Copyright (C) 2020 ~ 2021 Uniontech Technology Co., Ltd.
 *
 * Author:     xuyanghe <xuyanghe@uniontech.com>
 *
 * Maintainer: xuyanghe <xuyanghe@uniontech.com>
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

#include "gsettingwatcher.h"

#include <QGSettings>
#include <QVariant>
#include <QWidget>

GSettingWatcher::GSettingWatcher(QObject *parent)
    : QObject(parent)
    , m_gsettings(new QGSettings("com.deepin.dde.control-center", QByteArray(), this))
{
    connect(m_gsettings, &QGSettings::changed, this, &GSettingWatcher::onStatusModeChanged);
}

GSettingWatcher *GSettingWatcher::instance()
{
    static GSettingWatcher w;
    return &w;
}

void GSettingWatcher::bind(const QString &gsettingsName, QWidget *binder)
{
    m_map.insert(gsettingsName, binder);

    setStatus(gsettingsName, binder);
}

void GSettingWatcher::erase(const QString &gsettingsName)
{
    if (m_map.isEmpty() || !m_map.contains(gsettingsName))
        return;

    m_map.remove(gsettingsName);
}

void GSettingWatcher::erase(const QString &gsettingsName, QWidget *binder)
{
    if (m_map.isEmpty() || !m_map.contains(gsettingsName))
        return;

    m_map.remove(gsettingsName, binder);
}

void GSettingWatcher::setStatus(const QString &gsettingsName, QWidget *binder)
{
    if (!binder)
        return;

    const QString setting = m_gsettings->get(gsettingsName).toString();

    if ("Enabled" == setting)
        binder->setEnabled(true);
    else if ("Disabled" == setting)
        binder->setEnabled(false);

    binder->setVisible("Hiden" != setting);
}

const QString GSettingWatcher::getStatus(const QString &gsettingsName)
{
    return m_gsettings->get(gsettingsName).toString();
}

void GSettingWatcher::onStatusModeChanged(const QString &key)
{
    if (m_map.isEmpty() || !m_map.contains(key))
        return;

    // 重新设置控件对应的显示类型
    for (auto mapUnit = m_map.begin(); mapUnit != m_map.end(); ++mapUnit) {
        if (key == mapUnit.key()) {
            setStatus(key, mapUnit.value());
        }
    }
}
