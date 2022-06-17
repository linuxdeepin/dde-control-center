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

#include "adapterv20tov23module.h"
#include "layoutv20.h"
#include "moduleinterface.h"
#include <QEvent>
#include <QTimer>

AdapterV20toV23Child::AdapterV20toV23Child()
    : ModuleObject("adapterV20toV23Child")
    , m_widget(nullptr)
{
}

AdapterV20toV23Child::~AdapterV20toV23Child()
{
}

QWidget *AdapterV20toV23Child::page()
{
    return m_widget;
}
void AdapterV20toV23Child::setPage(QWidget *w)
{
    m_widget = w;
    widgetChanged();
}
////////////////////////
AdapterV20toV23Module::AdapterV20toV23Module(dccV20::ModuleInterface *v20Module)
    : ModuleObject(v20Module->name(), v20Module->displayName())
    , m_v20Module(v20Module)
{
    setChildType(LayoutV20_KEY);
    setIcon(m_v20Module->icon());
    m_modules[0] = new AdapterV20toV23Child();
    appendChild(m_modules[0]);
    m_modules[1] = new AdapterV20toV23Child();
    m_modules[0]->appendChild(m_modules[1]);
}

AdapterV20toV23Module::~AdapterV20toV23Module()
{
    delete m_v20Module;
}

dccV20::ModuleInterface *AdapterV20toV23Module::inter()
{
    return m_v20Module;
}

void AdapterV20toV23Module::active()
{
    m_v20Module->active();
}
// 第二级
QWidget *AdapterV20toV23Module::page()
{
    return m_v20Module->moduleWidget();
}

void AdapterV20toV23Module::deactive()
{
    m_v20Module->deactive();
}

QString AdapterV20toV23Module::path() const
{
    return m_v20Module->path();
}

QString AdapterV20toV23Module::follow() const
{
    return m_v20Module->follow();
}

bool AdapterV20toV23Module::enabled() const
{
    return m_v20Module->enabled();
}

void AdapterV20toV23Module::setChildPage(int level, QWidget *w)
{
    switch (level) {
    case 0:                             // 第三级
        m_modules[1]->setPage(nullptr); // 设置第三级时先清空第四级
    case 1:                             // 第四级
        m_modules[level]->setPage(w);
        break;
    default:
        break;
    }
}

void AdapterV20toV23Module::popWidget(QWidget *w)
{
    for (int i = 0; i < 2; i++) {
        if (m_modules[i]->page() == w)
            m_modules[i]->setPage(nullptr);
    }
}