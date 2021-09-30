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

#include "visiblemanagement.h"

#include <QVariant>
#include <QWidget>
#include <QListView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QJsonObject>
#include <QJsonDocument>

#include <QDebug>

const QString ModuleVisibleServer = "com.deepin.dde.ModuleVisible";
const QString ModuleVisiblePath = "/com/deepin/dde/ModuleVisible";

VisibleManagement::VisibleManagement(QObject *parent)
    : QObject(parent)
    , m_moduleVisble(new ModuleVisbleDbus(ModuleVisibleServer, ModuleVisiblePath, QDBusConnection::sessionBus(), this))
{
    initData();
    m_moduleVisble->setSync(false);
    connect(m_moduleVisble, &ModuleVisbleDbus::ModuleVisbleChanged, this, &VisibleManagement::onPageVisibleChanged, Qt::QueuedConnection);
    connect(m_moduleVisble, &ModuleVisbleDbus::VisibleChanged, this, &VisibleManagement::onAllDataChanged);
}


VisibleManagement *VisibleManagement::instance()
{
    static VisibleManagement w;
    return &w;
}

void VisibleManagement::bind(const QString &moduleName, QWidget *binder)
{
    m_map.insert(moduleName, binder);
    setStatus(binder, getStatus(moduleName));

    // 自动解绑
    connect(binder, &QObject::destroyed, this, [=]{
        erase(m_map.key(binder));
    });
}

void VisibleManagement::bind(const QString &moduleName, QListView *viewer, QStandardItem *item)
{
    m_menuMap.insert(moduleName, QPair<QListView *, QStandardItem *>(viewer, item));
    setStatus(viewer, item, getStatus(moduleName));
}

void VisibleManagement::erase(const QString &moduleName)
{
    if (m_map.isEmpty() || !m_map.contains(moduleName))
        return;

    m_map.remove(moduleName);
}

void VisibleManagement::erase(const QString &gsettingsName, QWidget *binder)
{
    if (m_map.isEmpty() || !m_map.contains(gsettingsName))
        return;

    m_map.remove(gsettingsName);
}

void VisibleManagement::setAppName(const QString &appName)
{
    if (m_appName.isEmpty())
       m_appName = appName;
    initData();
}

void VisibleManagement::clearMenuMap()
{
    m_menuMap.clear();
}

void VisibleManagement::setStatus(QWidget *binder, const bool visible)
{
    binder->setVisible(visible);
}

void VisibleManagement::initData()
{
    m_Visibledata.clear();
    if (m_moduleVisble->isValid()) {
        //初始化模块显示隐藏数据
        initDataFromDbus(m_moduleVisble->visible());
    } else {
        //当dbus服务没有启动的时候，则去conf文件中获取数据
        initDataFromConfFile();
    }

    //初始化搜索数据
    initSeartchData();

    //初始化显示隐藏状态
    initStatus();

}

void VisibleManagement::setStatus(QListView *viewer, QStandardItem *item, const bool visible)
{
    viewer->setRowHidden(item->row(), !visible);

    if (!visible) {
        Q_EMIT requestUpdateSecondMenu(item->row());
        if (viewer->currentIndex().row() == item->row())
            Q_EMIT requestCurrentPageOff();
    }
}


bool VisibleManagement::getStatus(const QString &moduleName)
{
    return !m_Visibledata.contains(moduleName) ? true : m_Visibledata.value(moduleName);
}

void VisibleManagement::initDataFromDbus(const QString &visibleData)
{
    updateData(visibleData);
}

void VisibleManagement::initSeartchData()
{
    QStringList moudleNames = m_Visibledata.keys();
    for (auto modulename: moudleNames) {
        QString pageName = modulename.split("_")[1];
        m_seartchMap.insert(pageName, m_Visibledata.value(modulename));
    }
    Q_EMIT requestSearchMenuAll();
}

void VisibleManagement::initStatus()
{

    QStringList moduleKeys = m_Visibledata.keys();
    QStringList widgetKeys = m_map.keys();
    QStringList viewKeys = m_menuMap.keys();

    for (QString moduleName : moduleKeys) {
        if (widgetKeys.contains(moduleName)) {
            setStatus(m_map.value(moduleName), m_Visibledata.value(moduleName));
        } else if (viewKeys.contains(moduleName)) {
            setStatus(m_menuMap.value(moduleName).first, m_menuMap.value(moduleName).second, m_Visibledata.value(moduleName));
        }
    }

    Q_EMIT requestDataChanged();

}

void VisibleManagement::onPageVisibleChanged(const QString &appName, const QString &moduleName, const QString &pageName, const bool enable)
{

    if (appName != m_appName) return;


    QString key = moduleName + "_" + pageName;
    m_Visibledata[key] = enable;

    QStringList widgetKeys = m_map.keys();
    QStringList viewKeys = m_menuMap.keys();

    //开关搜索字段
    if (m_seartchMap.keys().contains(pageName)) {
        m_seartchMap[pageName] = enable;
        Q_EMIT requestUpdateSearchMenu(pageName, enable);
    }

    if (widgetKeys.contains(key)) {
        setStatus(m_map.value(key), enable);
    } else if (viewKeys.contains(key)) {
        setStatus(m_menuMap.value(key).first, m_menuMap.value(key).second, enable);
    }
    if (moduleName == "Module")
        Q_EMIT requestDataChanged();
}

void VisibleManagement::onAllDataChanged(const QString &data)
{
     initDataFromDbus(data);
     //初始化搜索数据
     initSeartchData();
     //初始化显示隐藏状态
     initStatus();
}

void VisibleManagement::updateData(const QString &data)
{
    //当没设置appName则找不到相应的配置程序，所以默认为显示状态
    if (m_appName.isEmpty() || !data.contains(m_appName)) return;

    QJsonObject jsonData = QJsonDocument::fromJson(data.toUtf8()).object()[m_appName].toObject();
    qInfo() << "jsonData" << jsonData;
    //由于前面判空了。所以这里一定可以取到
    QStringList modules = jsonData.keys();
    m_Visibledata.clear();
    for (QString moduleName: modules) {
        //这里默认值采用显示，而不是隐藏，防止没有配置的被隐藏
        m_Visibledata.insert(moduleName, jsonData[moduleName].toBool(true));
    }
}

void VisibleManagement::initDataFromConfFile()
{
    QString confPath = "/etc/deepin/ModuleVisible/" + m_appName + ".conf";
    QSettings setting(confPath, QSettings::IniFormat);
    QStringList Groups = setting.childGroups();
    for (QString group : Groups) {
        setting.beginGroup(group);
        QStringList keys = setting.allKeys();
        for (QString key : keys) {
            m_Visibledata.insert(group + "_" + key, setting.value(key, true).toBool());
        }
        setting.endGroup();
    }
}
