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
#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H
#include "interface/layoutbase.h"

DCC_BEGIN_NAMESPACE
class LayoutManager
{
public:
    LayoutManager();
    ~LayoutManager();
    LayoutBase *createLayout(DCC_LAYOUT_TYPE type);
    bool registerLayout(LayoutFactoryBase *factory);
    void registerLayout(QList<LayoutFactoryBase *> factories);

private:
    QMap<DCC_LAYOUT_TYPE, LayoutFactoryBase *> m_mapFactory;
};
DCC_END_NAMESPACE

#endif // LAYOUTMANAGER_H
