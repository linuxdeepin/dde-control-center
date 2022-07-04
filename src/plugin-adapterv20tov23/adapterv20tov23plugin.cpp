/*
 * Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
 *
 * Author:     caixiangrong <caixiangrong@uniontech.com>
 *
 * Maintainer: caixiangrong <caixiangrong@uniontech.com>
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

#include "adapterv20tov23plugin.h"
#include "adapterv20tov23module.h"
#include "moduleinterface.h"
#include "frameproxyv20.h"
#include "pluginmanagerv20.h"
#include "dbuscontrolcenterservice.h"

#include <QEvent>
#include <QTimer>
#include <qdbusconnection.h>
#include <qdebug.h>

AdapterV20toV23Root::AdapterV20toV23Root()
    : ModuleObject("adapterV20toV23")
    , m_root(nullptr)
    , m_timer(new QTimer(this))
    , m_prameProxy(nullptr)
    , m_pluginManagerV20(nullptr)
    , m_status(WaitParent)
    , m_tryCount(50)
{
    setHiden(true);
    connect(m_timer, &QTimer::timeout, this, &AdapterV20toV23Root::timerTask);
    m_timer->start(10);
    registerDBus();
}

AdapterV20toV23Root::~AdapterV20toV23Root()
{
    if (m_timer)
        delete m_timer;
    if (m_pluginManagerV20)
        delete m_pluginManagerV20;
    if (m_prameProxy)
        delete m_prameProxy;
}

bool AdapterV20toV23Root::loadFinished() const
{
    return m_status == LoadFinished;
}

dccV23::ModuleObject *AdapterV20toV23Root::moduleRoot() const
{
    return m_root;
}

void AdapterV20toV23Root::timerTask()
{
    switch (m_status) {
    case WaitParent: // 等待获取父对象
        m_root = getParent();
        if (m_root)
            m_status = GetPaths;
        break;
    case GetPaths: // 获取插件路径，并完成一些初始化
        m_root->removeChild(this);
        connect(m_root, &ModuleObject::destroyed, this, &AdapterV20toV23Root::deleteLater);
        connect(m_root, &ModuleObject::appendedChild, this, &AdapterV20toV23Root::pushModule, Qt::QueuedConnection);
        connect(m_root, &ModuleObject::insertedChild, this, &AdapterV20toV23Root::pushModule, Qt::QueuedConnection);
        m_prameProxy = new FrameProxyV20(this);
        m_prameProxy->setRootModule(m_root);
        m_pluginManagerV20 = new PluginManagerV20();
        m_pluginPaths = m_pluginManagerV20->pluginPath();
        m_status = LoadPlugin;
        break;
    case LoadPlugin: // 加载插件，每次加载一个
        if (!m_pluginPaths.isEmpty())
            m_pluginManagerV20->loadPlugin(m_pluginPaths.takeFirst(), m_prameProxy);
        else {
            m_modules = m_pluginManagerV20->modules();
            delete m_pluginManagerV20;
            m_pluginManagerV20 = nullptr;
            m_status = InsertModule;
        }
        break;
    case InsertModule: // 尝试插入
        pushModule();
        m_tryCount--;
        if (m_modules.isEmpty() || m_tryCount <= 0)
            m_status = InsertAllModule;
        break;
    case InsertAllModule: // 插入所有项，并断开信号
        disconnect(m_root, &ModuleObject::appendedChild, this, &AdapterV20toV23Root::pushModule);
        disconnect(m_root, &ModuleObject::insertedChild, this, &AdapterV20toV23Root::pushModule);
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
        insertModule(true);
        m_status = LoadFinished;
        break;
    default:
        break;
    }
}

void AdapterV20toV23Root::pushModule()
{
    insertModule(false);
}

void AdapterV20toV23Root::insertModule(bool append)
{
    while (!m_modules.isEmpty()) {
        auto &&module = m_modules.takeFirst();
        QString path = module->path();
        QString follow = module->follow();
        ModuleObject *root = m_root;
        if (path != "mainwindow") {
            auto it = std::find_if(m_root->childrens().begin(), m_root->childrens().end(), [path](auto &&child) {
                return child->name() == path;
            });

            if (it == m_root->childrens().end()) {
                if (append) {
                    follow = QString::number(m_root->getChildrenSize() + 1);
                    root = m_root;
                } else {
                    m_modules.append(module);
                    return;
                }
            } else {
                root = (*it);
            }
        }
        bool ok;
        int index = follow.toInt(&ok) - 1;
        if (!ok) {
            index = -1;
            for (int i = 0; index == -1 && i < root->getChildrenSize(); i++) {
                if (root->children(i)->name() == follow)
                    index = i + 1;
            }
        }
        if (index == -1) {
            if (append) {
                index = m_root->getChildrenSize();
            } else {
                m_modules.append(module);
                return;
            }
        }
        root->insertChild(index, module);

        m_prameProxy->append(module);
        dccV20::ModuleInterface *inter = module->inter();
        inter->setFrameProxy(m_prameProxy);
        if (inter->follow() != MAINWINDOW && m_prameProxy) {
            m_prameProxy->setSearchPath(inter);
        }
        inter->preInitialize(false);
        inter->initialize();
    }
}

void AdapterV20toV23Root::registerDBus()
{
    DBusControlCenterService *dbus = new DBusControlCenterService(this);
    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.registerService("com.deepin.dde.ControlCenter") ||
        !conn.registerObject("/com/deepin/dde/ControlCenter", dbus, QDBusConnection::ExportAllSlots)) {
        qDebug() << "can't regist dbus!";
    }
}

///////////////////////////
AdapterV20toV23Plugin::AdapterV20toV23Plugin(QObject *parent)
    : PluginInterface(parent)
    , m_moduleRoot(nullptr)
{
}

AdapterV20toV23Plugin::~AdapterV20toV23Plugin()
{
}

QString AdapterV20toV23Plugin::name() const
{
    return "adapterv20tov23";
}

DCC_NAMESPACE::ModuleObject *AdapterV20toV23Plugin::module()
{
    return new AdapterV20toV23Root();
}
