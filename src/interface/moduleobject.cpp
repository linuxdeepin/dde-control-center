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

#include <DObjectPrivate>

#define DCC_HIDDEN 0x80000000
#define DCC_DISABLED 0x40000000

/** Versions:
    V1.0 - 2022/05/24 - create
**/
const unsigned c_currentVersion = 10; //1.0

//DCORE_USE_NAMESPACE

DCC_USE_NAMESPACE
DCC_BEGIN_NAMESPACE
class ModuleObjectPrivate : public DTK_CORE_NAMESPACE::DObjectPrivate
{
public:
    explicit ModuleObjectPrivate(ModuleObject *object)
        : DObjectPrivate(object)
        , m_badge(0)
        , m_flags(0)
        , m_childType(ModuleObject::Main)
    {
    }

public:
    QString m_name;            // 名称，作为每个模块的唯一标识，不可为空
    QString m_displayName;     // 显示名称，如菜单的名称，页面的标题等，为空则不显示
    QString m_description;     // 描述，如主菜单的描述信息
    QStringList m_contentText; // 上下文数据，参与搜索，只可用于终结点：DisplayName -> ContentText(one of it)
    QIcon m_icon;              // 图标，如主菜单的图标
    int m_badge;               // 主菜单中的角标, 默认为0不显示，大于0显示

    uint32_t m_flags;
    DCC_LAYOUT_TYPE m_childType;
    QList<ModuleObject *> m_childrens;
};
DCC_END_NAMESPACE
ModuleObject::ModuleObject(QObject *parent)
    : ModuleObject(QString(), QString(), parent)
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
    , DObject(*new ModuleObjectPrivate(this), this)
{
    D_D(ModuleObject);
    d->m_name = name;
    d->m_displayName = displayName;
    d->m_description = description;
    d->m_contentText = contentText;
    d->m_icon = icon;
}

ModuleObject::~ModuleObject()
{
    deactive();
}

QString ModuleObject::name() const
{
    D_D(const ModuleObject);
    return d->m_name;
}
QString ModuleObject::displayName() const
{
    D_D(const ModuleObject);
    return d->m_displayName;
}
QString ModuleObject::description() const
{
    D_D(const ModuleObject);
    return d->m_description;
}
QStringList ModuleObject::contentText() const
{
    D_D(const ModuleObject);
    return d->m_contentText;
}
QIcon ModuleObject::icon() const
{
    D_D(const ModuleObject);
    return d->m_icon;
}
int ModuleObject::badge() const
{
    D_D(const ModuleObject);
    return d->m_badge;
}

bool ModuleObject::isHiden() const
{
    return getFlagState(DCC_HIDDEN);
}

bool ModuleObject::isDisabled() const
{
    return getFlagState(DCC_DISABLED);
}

unsigned ModuleObject::GetCurrentVersion()
{
    return c_currentVersion;
}

void ModuleObject::setHiden(bool hiden)
{
    setFlagState(DCC_HIDDEN, hiden);
}

void ModuleObject::setDisabled(bool disabled)
{
    setFlagState(DCC_DISABLED, disabled);
}

void ModuleObject::trigger()
{
    Q_EMIT triggered();
}

void ModuleObject::setName(const QString &name)
{
    D_D(ModuleObject);
    if (d->m_name != name) {
        d->m_name = name;
        Q_EMIT moduleDataChanged();
    }
}
void ModuleObject::setDisplayName(const QString &displayName)
{
    D_D(ModuleObject);
    if (d->m_displayName != displayName) {
        d->m_displayName = displayName;
        Q_EMIT moduleDataChanged();
    }
}
void ModuleObject::setDescription(const QString &description)
{
    D_D(ModuleObject);
    if (d->m_description != description) {
        d->m_description = description;
        Q_EMIT moduleDataChanged();
    }
}
void ModuleObject::setContentText(const QStringList &contentText)
{
    D_D(ModuleObject);
    if (d->m_contentText != contentText) {
        d->m_contentText = contentText;
        Q_EMIT moduleDataChanged();
    }
}

void ModuleObject::addContentText(const QString &contentText)
{
    D_D(ModuleObject);
    d->m_contentText << contentText;
    Q_EMIT moduleDataChanged();
}

void ModuleObject::addContentText(const QStringList &contentText)
{
    D_D(ModuleObject);
    d->m_contentText << contentText;
    Q_EMIT moduleDataChanged();
}

void ModuleObject::setIcon(const QIcon &icon)
{
    D_D(ModuleObject);
    d->m_icon = icon;
    Q_EMIT moduleDataChanged();
}
void ModuleObject::setBadge(int badge)
{
    D_D(ModuleObject);
    if (d->m_badge != badge) {
        d->m_badge = badge;
        Q_EMIT moduleDataChanged();
    }
}

void ModuleObject::appendChild(ModuleObject *const module)
{
    D_D(ModuleObject);
    if (d->m_childrens.contains(module))
        return;
    d->m_childrens.append(module);
    module->setParent(this);
    Q_EMIT appendedChild(module);
    Q_EMIT childrenSizeChanged(d->m_childrens.size());
}

void ModuleObject::removeChild(ModuleObject *const module)
{
    D_D(ModuleObject);
    if (!d->m_childrens.contains(module))
        return;
    Q_EMIT removedChild(module);
    d->m_childrens.removeOne(module);
    Q_EMIT childrenSizeChanged(d->m_childrens.size());
}

void ModuleObject::removeChild(const int index)
{
    D_D(ModuleObject);
    if (d->m_childrens.size() <= index)
        return;
    Q_EMIT removedChild(d->m_childrens[index]);
    d->m_childrens.removeAt(index);
    Q_EMIT childrenSizeChanged(d->m_childrens.size());
}

void ModuleObject::insertChild(QList<ModuleObject *>::iterator before, ModuleObject *const module)
{
    D_D(ModuleObject);
    if (d->m_childrens.contains(module))
        return;
    d->m_childrens.insert(before, module);
    module->setParent(this);
    Q_EMIT insertedChild(module);
    Q_EMIT childrenSizeChanged(d->m_childrens.size());
}

void ModuleObject::insertChild(const int index, ModuleObject *const module)
{
    D_D(ModuleObject);
    if (d->m_childrens.contains(module))
        return;
    d->m_childrens.insert(index, module);
    module->setParent(this);
    Q_EMIT insertedChild(module);
    Q_EMIT childrenSizeChanged(d->m_childrens.size());
}

bool ModuleObject::getFlagState(uint32_t flag) const
{
    D_D(const ModuleObject);
    return (d->m_flags & flag);
}

uint32_t ModuleObject::getFlag() const
{
    D_D(const ModuleObject);
    return d->m_flags;
}

void ModuleObject::setFlagState(uint32_t flag, bool state)
{
    D_D(ModuleObject);
    if (getFlagState(flag) != state) {
        if (state)
            d->m_flags |= flag;
        else
            d->m_flags &= (~flag);
        Q_EMIT stateChanged(flag, state);
        ModuleObject *parent = getParent();
        if (parent)
            Q_EMIT parent->childStateChanged(this, flag, state);
    }
}

ModuleObject *ModuleObject::getParent()
{
    return dynamic_cast<ModuleObject *>(parent());
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

DCC_LAYOUT_TYPE ModuleObject::childType() const
{
    D_D(const ModuleObject);
    return d->m_childType;
}

void ModuleObject::setChildType(const DCC_LAYOUT_TYPE &t)
{
    D_D(ModuleObject);
    d->m_childType = t;
}

const QList<ModuleObject *> &ModuleObject::childrens()
{
    D_D(const ModuleObject);
    return d->m_childrens;
}

ModuleObject *ModuleObject::children(const int index) const
{
    D_D(const ModuleObject);
    if (index < 0 || index >= d->m_childrens.size())
        return nullptr;
    return d->m_childrens.at(index);
}

int ModuleObject::getChildrenSize() const
{
    D_D(const ModuleObject);
    return d->m_childrens.size();
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
