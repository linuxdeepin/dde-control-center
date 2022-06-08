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
#include "layoutmanager.h"
#include "mainlayout.h"
#include "hlistlayout.h"
#include "vlistlayout.h"
#include "pagelayout.h"

DCC_USE_NAMESPACE
LayoutManager::LayoutManager()
{
}

LayoutManager::~LayoutManager()
{
    QMap<DCC_LAYOUT_TYPE, LayoutFactoryBase *>::const_iterator iter = m_mapFactory.cbegin();
    while (iter != m_mapFactory.cend()) {
        delete iter.value();
        ++iter;
    }
}

LayoutBase *LayoutManager::createLayout(DCC_LAYOUT_TYPE type)
{
    LayoutBase *layout = nullptr;
    if (m_mapFactory.contains(type)) {
        layout = m_mapFactory.value(type)->createLayout();
    }
    if (!layout) {
        switch (type & 0xFF000000) {
        case ModuleObject::Main:
            layout = new MainLayout;
            break;
        case ModuleObject::HList:
            layout = new HListLayout;
            break;
        case ModuleObject::VList:
            layout = new VListLayout;
            break;
        case ModuleObject::Page:
        default:
            layout = new PageLayout;
            break;
        }
    }
    return layout;
}

bool LayoutManager::registerLayout(LayoutFactoryBase *factory)
{
    if (!m_mapFactory.contains(factory->type())) {
        m_mapFactory.insert(factory->type(), factory);
        return true;
    }
    qWarning() << "layout type:" << QString::number(factory->type(), 16) << "have been used";
    return false;
}

void LayoutManager::registerLayout(QList<LayoutFactoryBase *> factories)
{
    for (auto &&factory : factories) {
        registerLayout(factory);
    }
}
