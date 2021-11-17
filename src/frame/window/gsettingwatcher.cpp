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
#include <QDebug>

GSettingWatcher::GSettingWatcher(QObject *parent)
    : QObject(parent)
    , m_gsettings(new QGSettings("com.deepin.dde.control-center", QByteArray(), this))
    , m_keys(m_gsettings->keys())
{
    connect(m_gsettings, &QGSettings::changed, this, &GSettingWatcher::onStatusModeChanged);
}

GSettingWatcher *GSettingWatcher::instance()
{
    static GSettingWatcher w;
    return &w;
}

bool GSettingWatcher::existKey(const QString &key)
{
    if (!m_keys.contains(key)) {
        qWarning() <<"key:" << key << "non-existent!";
        return false;
    }

    return true;
}

/**
 * @brief GSettingWatcher::bind 三级控件绑定gsettings
 * @param gsettingsName         key值
 * @param binder                控件指针
 */
void GSettingWatcher::bind(const QString &gsettingsName, QWidget *binder)
{
    m_map.insert(gsettingsName, binder);

    setStatus(gsettingsName, binder);

    // 自动解绑
    connect(binder, &QObject::destroyed, this, [=] {
        erase(m_map.key(binder));
    });
}

/**
 * @brief GSettingWatcher::bind 二级菜单绑定gsettings
 * @param gsettingsName         key值
 * @param viewer                listview指针
 * @param item                  item指针
 */
void GSettingWatcher::bind(const QString &gsettingsName, QListView *viewer, QStandardItem *item)
{
    m_menuMap.insert(gsettingsName, QPair<QListView *, QStandardItem *>(viewer, item));

    setStatus(gsettingsName, viewer, item);

    connect(viewer, &QListView::destroyed, this, [=] {
        erase(gsettingsName);
    });
}

/**
 * @brief GSettingWatcher::erase 清楚map中已被析构的值
 * @param gsettingsName          key值
 */
void GSettingWatcher::erase(const QString &gsettingsName)
{
    if (!m_map.isEmpty() && m_map.contains(gsettingsName)) {
        m_map.remove(gsettingsName);
    }

    if (!m_menuMap.isEmpty() && m_menuMap.contains(gsettingsName)) {
        m_menuMap.remove(gsettingsName);
    }
}

/**
 * @brief GSettingWatcher::erase erase重载，指定删除特定key
 * @param gsettingsName          key值
 * @param binder                 控件指针
 */
void GSettingWatcher::erase(const QString &gsettingsName, QWidget *binder)
{
    if (m_map.isEmpty() || !m_map.contains(gsettingsName))
        return;

    m_map.remove(gsettingsName, binder);
}

/**
 * @brief GSettingWatcher::insertState 插入二级菜单初始状态
 * @param key                          key值
 */
void GSettingWatcher::insertState(const QString &key)
{
    if(!existKey(key))
        return;

    m_menuState.insert(key, m_gsettings->get(key).toBool());
}

/**
 * @brief GSettingWatcher::setStatus 设置三级控件状态
 * @param gsettingsName              key值
 * @param binder                     控件指针
 */
void GSettingWatcher::setStatus(const QString &gsettingsName, QWidget *binder)
{
    if (!binder || !existKey(gsettingsName))
        return;

    const QString setting = m_gsettings->get(gsettingsName).toString();

    if ("Enabled" == setting) {
        binder->setEnabled(true);
    } else if ("Disabled" == setting) {
        binder->setEnabled(false);
    }

    binder->setVisible("Hidden" != setting);
}

/**
 * @brief GSettingWatcher::setStatus 设置二级菜单状态
 * @param gsettingsName              key值
 * @param viewer                     listview指针
 * @param item                       item指针
 */
void GSettingWatcher::setStatus(const QString &gsettingsName, QListView *viewer, QStandardItem *item)
{
    if(!existKey(gsettingsName))
        return;

    bool visible = m_gsettings->get(gsettingsName).toBool();

    viewer->setRowHidden(item->row(), !visible);

    if(visible)
        Q_EMIT requestShowSecondMenu(item->row());
    else
        Q_EMIT requestUpdateSecondMenu(item->row());
}

/**
 * @brief GSettingWatcher::getStatus 获取三级控件状态
 * @param gsettingsName              key值
 * @return
 */
const QString GSettingWatcher::getStatus(const QString &gsettingsName)
{
    if(!existKey(gsettingsName))
        return QString();

    return m_gsettings->get(gsettingsName).toString();
}

/**
 * @brief GSettingWatcher::getMenuState
 *
 * @return second menu state
 */
QMap<QString, bool> GSettingWatcher::getMenuState()
{
    return m_menuState;
}

/**
 * @brief 设置控件对应的显示类型
 *
 * @param key
 */
void GSettingWatcher::onStatusModeChanged(const QString &key)
{
    if (!m_map.isEmpty() && m_map.contains(key)) {
        // 重新设置控件对应的显示类型
        for (auto mapUnit = m_map.begin(); mapUnit != m_map.end(); ++mapUnit) {
            if (key == mapUnit.key()) {
                setStatus(key, mapUnit.value());
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

    if (!m_menuState.isEmpty() && m_menuState.keys().contains(key)) {
        insertState(key);
        Q_EMIT requestUpdateSearchMenu(key, m_menuState.value(key));
    }
}
