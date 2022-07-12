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
    if (m_widget)
        m_widget->setVisible(true);

    return m_widget;
}

void AdapterV20toV23Child::setPage(QWidget *w)
{
    if (w == m_widget)
        return;

    m_widget = w;
    if (m_widget)
        m_widget->setVisible(false);

    widgetChanged();
}
////////////////////////
AdapterV20toV23Module::AdapterV20toV23Module(dccV20::ModuleInterface *v20Module)
    : AdapterV20toV23Child()
    , m_v20Module(v20Module)
{
    setChildType(LayoutV20_KEY);
    setName(v20Module->name());
    setDisplayName(v20Module->displayName());
    setIcon(m_v20Module->icon());

    m_modules.append(this);
    for (int i = 0; i < 3; i++) {
        AdapterV20toV23Child *child = new AdapterV20toV23Child();
        m_modules.last()->appendChild(child);
        m_modules.append(child);
    }
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
    emit actived();
    m_v20Module->active();
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
    if (level < m_modules.size())
        m_modules.at(level)->setPage(w);

    for (int i = m_modules.size() - 1; i > level; --i) {
        m_modules.at(i)->setPage(nullptr);
    }
}

void AdapterV20toV23Module::popWidget(QWidget *w)
{
    for (auto &&child : m_modules) {
        if (child->widget() == w)
            child->setPage(nullptr);
    }
}
