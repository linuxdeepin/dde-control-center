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
#include <QListView>
#include <QStandardItem>
#include <QStandardItemModel>
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

    // 自动解绑
    connect(binder, &QObject::destroyed, this, [=] {
        erase(m_map.key(binder));
    });
}

void GSettingWatcher::bind(const QString &gsettingsName, QListView *viewer, QStandardItem *item)
{
    m_menuMap.insert(gsettingsName, QPair<QListView *, QStandardItem *>(viewer, item));

    setStatus(gsettingsName, viewer, item);

    connect(viewer, &QObject::destroyed, this, [=] {
        erase(gsettingsName);
    });
}

void GSettingWatcher::erase(const QString &gsettingsName)
{
    if (!m_map.isEmpty() && m_map.contains(gsettingsName)) {
        m_map.remove(gsettingsName);
    }

    if (!m_menuMap.isEmpty() && m_menuMap.contains(gsettingsName)) {
        m_menuMap.remove(gsettingsName);
    }

}

void GSettingWatcher::erase(const QString &gsettingsName, QWidget *binder)
{
    if (m_map.isEmpty() || !m_map.contains(gsettingsName))
        return;

    m_map.remove(gsettingsName, binder);
}

void GSettingWatcher::clearMenuMap()
{
    // m_menuMap.clear();
}

void GSettingWatcher::setStatus(const QString &gsettingsName, QWidget *binder)
{
    if (!binder)
        return;

    const QString setting = m_gsettings->get(gsettingsName).toString();

    if ("Enabled" == setting) {
        binder->setEnabled(true);
    } else if ("Disabled" == setting) {
        binder->setEnabled(false);
    }

    binder->setVisible("Hidden" != setting);
}

void GSettingWatcher::setStatus(const QString &gsettingsName, QListView *viewer, QStandardItem *item)
{
    bool visible = m_gsettings->get(gsettingsName).toBool();

    viewer->setRowHidden(item->row(), !visible);

    if (!visible) {
        Q_EMIT requestUpdateSecondMenu(item->row());
    }
}

const QString GSettingWatcher::getStatus(const QString &gsettingsName)
{
    return m_gsettings->get(gsettingsName).toString();
}

QMap<QString, bool> GSettingWatcher::getMenuState()
{
    QMap<QString, bool> menuStates;

    // commoninfo
    menuStates.insert("bootMenu", m_gsettings->get("bootMenu").toBool());
    menuStates.insert("developerMode", m_gsettings->get("developerMode").toBool());
    menuStates.insert("userExperienceProgram", m_gsettings->get("userExperienceProgram").toBool());

    // datetime
    menuStates.insert("timezoneList", m_gsettings->get("timezoneList").toBool());
    menuStates.insert("timeSettings", m_gsettings->get("timeSettings").toBool());
    menuStates.insert("timeFormat", m_gsettings->get("timeFormat").toBool());
    /* sound module */
    menuStates.insert("soundInput", m_gsettings->get("soundInput").toBool());
    menuStates.insert("soundOutput", m_gsettings->get("soundOutput").toBool());
    menuStates.insert("soundEffects", m_gsettings->get("soundEffects").toBool());
    /* power module */
    menuStates.insert("general", m_gsettings->get("general").toBool());
    menuStates.insert("pluggedIn", m_gsettings->get("pluggedIn").toBool());
    menuStates.insert("onBattery", m_gsettings->get("onBattery").toBool());
    /* systeminfo */
    menuStates.insert("aboutThisPc", m_gsettings->get("aboutThisPc").toBool());
    menuStates.insert("editionLicense", m_gsettings->get("editionLicense").toBool());
    menuStates.insert("endUserLicenseAgreement", m_gsettings->get("endUserLicenseAgreement").toBool());
    menuStates.insert("privacyPolicy", m_gsettings->get("privacyPolicy").toBool());
    /* notification */
    menuStates.insert("systemNotification", m_gsettings->get("systemNotification").toBool());
    menuStates.insert("appNotifications", m_gsettings->get("appNotifications").toBool());
    return menuStates;
}

/**
 * @brief 设置控件对应的显示类型
 *
 * @param key
 */
void GSettingWatcher::onStatusModeChanged(const QString &key)
{
    if (!m_map.isEmpty() && m_map.contains(key)) {
        for (QString &nameKey : m_map.keys()) {
            if (key == nameKey) {
                setStatus(key, m_map.value(nameKey));
                break;
            }
        }
    }

    if (!m_menuMap.isEmpty() && m_menuMap.contains(key)) {
        for (QString &nameKey : m_menuMap.keys()) {
            if (key == nameKey) {
                setStatus(key, m_menuMap.value(nameKey).first, m_menuMap.value(nameKey).second);
                break;
            }
        }
    }

    QMap<QString, bool> map = getMenuState();
    if (map.keys().contains(key)) {
        Q_EMIT requestUpdateSearchMenu(key, map.value(key));
    }
}
