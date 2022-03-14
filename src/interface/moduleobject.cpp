/*
* Copyright (C) 2019 ~ 2022 Deepin Technology Co., Ltd.
*
* Author:     guoyao <guoyao@uniontech.com>

* Maintainer: guoyao <guoyao@uniontech.com>

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "interface/moduleobject.h"

DCC_USE_NAMESPACE

ModuleObject::ModuleObject(QObject *parent)
    : QObject(parent)
    , m_moduleData(new ModuleData(this))
    , m_childType(ChildType::Box)
{
}

ModuleObject::ModuleObject(const QString &name, const QString &displayName, QObject *parent)
    : ModuleObject(name, displayName, QStringList(), parent)
{

}

ModuleObject::ModuleObject(const QString &name, const QStringList &contentText, QObject *parent)
    : ModuleObject(name, {}, contentText, parent)
{

}

ModuleObject::ModuleObject(const QString &name, const QString &displayName, const QStringList &contentText, QObject *parent)
    : ModuleObject(name, displayName, {}, contentText, QIcon(), parent)
{

}

ModuleObject::ModuleObject(const QString &name, const QString &displayName, const QIcon &icon, QObject *parent)
    : ModuleObject(name, displayName, {}, icon, parent)
{

}

ModuleObject::ModuleObject(const QString &name, const QString &displayName, const QString &description, const QIcon &icon, QObject *parent)
    : ModuleObject(name, displayName, description, {}, icon, parent)
{

}

ModuleObject::ModuleObject(const QString &name, const QString &displayName, const QString &description, const QStringList &contentText, const QIcon &icon, QObject *parent)
    : QObject(parent)
    , m_moduleData(new ModuleData(name, displayName, description, contentText, icon, this))
    , m_childType(ChildType::Box)
{

}

void ModuleObject::setModuleData(ModuleData *data) {
    if (m_moduleData != data) {
        m_moduleData->deleteLater();
    }
    m_moduleData = data;
    data->setParent(this);
    Q_EMIT moduleDataChanged(data);
}

void ModuleObject::appendChild(ModuleObject *const module) {
    if (m_childrens.contains(module))
        return;
    m_childrens.append(module);
    module->setParent(this);
    Q_EMIT appendedChild(module);
    Q_EMIT childrenSizeChanged(m_childrens.size());
}

void ModuleObject::removeChild(ModuleObject *const module) {
    if (!m_childrens.contains(module))
        return;
    Q_EMIT removedChild(module);
    m_childrens.removeOne(module);
    Q_EMIT childrenSizeChanged(m_childrens.size());
}

void ModuleObject::removeChild(const int index)
{
    if (m_childrens.size() <= index)
        return;
    Q_EMIT removedChild(m_childrens[index]);
    m_childrens.removeAt(index);
    Q_EMIT childrenSizeChanged(m_childrens.size());
}

void ModuleObject::insertChild(QList<ModuleObject *>::iterator before, ModuleObject *const module) {
    if (m_childrens.contains(module))
        return;
    m_childrens.insert(before, module);
    module->setParent(this);
    Q_EMIT insertedChild(module);
    Q_EMIT childrenSizeChanged(m_childrens.size());
}

void ModuleObject::insertChild(const int index, ModuleObject *const module)
{
    if (m_childrens.contains(module))
        return;
    m_childrens.insert(index, module);
    module->setParent(this);
    Q_EMIT insertedChild(module);
    Q_EMIT childrenSizeChanged(m_childrens.size());
}

int ModuleObject::findChild(ModuleObject *const child)
{
    if (!child)
        return -1;
    if (this == child)
        return 0;
    return findChild(this, child, 0);
}

int ModuleObject::findChild(ModuleObject *const module, ModuleObject *const child)
{
    if (!module || !child)
        return -1;
    if (module == child)
        return 0;
    return findChild(module, child, 0);
}

int ModuleObject::findChild(ModuleObject *const module, ModuleObject *const child, const int num)
{
    for (auto ch : module->childrens()) {
        if (ch == child)
            return num + 1;
    }
    for (auto ch : module->childrens()) {
        const int temp = ModuleObject::findChild(ch, child, num + 1);
        if (temp > num)
            return temp;
    }
    return -1;
}
