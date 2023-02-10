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

#include "moduleinterface.h"

#include <QEvent>
#include <QHBoxLayout>

AdapterV20toV23Module::AdapterV20toV23Module(dccV20::ModuleInterface *v20Module)
    : ModuleObject()
    , m_v20Module(v20Module)
    , m_layout(nullptr)
{
    setName(v20Module->name());
    setDisplayName(v20Module->displayName());
    setIcon(m_v20Module->icon());
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
    m_layout = nullptr;
    for (auto &&w : m_widgets) {
        w = nullptr;
    }

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

void AdapterV20toV23Module::setWidget(int index)
{
    if (!m_layout)
        return;

    while (m_layout->count() > index) {
        QLayoutItem *item = m_layout->takeAt(index);
        QWidget *oldW = item->widget();
        if (!m_widgets.contains(oldW)) {
            oldW->setVisible(false);
            oldW->setParent(nullptr);
            oldW->deleteLater();
        }
        delete item;
    }

    for (int i = index; i < m_widgets.count(); ++i) {
        m_layout->addWidget(m_widgets.at(i), i * 4 + 3);
    }
}

void AdapterV20toV23Module::setChildPage(int level, QWidget *w)
{
    while (level < m_widgets.size()) {
        m_widgets.takeLast();
    }
    m_widgets.append(w);
    setWidget(level);
}

void AdapterV20toV23Module::popWidget(QWidget *w)
{
    while (!m_widgets.isEmpty()) {
        QWidget *last = m_widgets.takeLast();
        if (last == w)
            break;
    }
    setWidget(0);
}

QWidget *AdapterV20toV23Module::page()
{
    QWidget *parentWidget = new QWidget();
    m_layout = new QHBoxLayout();
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    parentWidget->setLayout(m_layout);
    setWidget(0);

    return parentWidget;
}
