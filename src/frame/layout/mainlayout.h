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
#ifndef MAINLAYOUT_H
#define MAINLAYOUT_H
#include "interface/layoutbase.h"
DCC_BEGIN_NAMESPACE
class ListView;
class ModuleDataModel;
class MainLayout : public LayoutBase
{
public:
    MainLayout();

    DCC_NAMESPACE::ModuleObject *autoExpand(DCC_NAMESPACE::ModuleObject *const module, DCC_NAMESPACE::ModuleObject *const child) override;
    void setCurrent(DCC_NAMESPACE::ModuleObject *const child) override;
    QWidget *layoutModule(DCC_NAMESPACE::ModuleObject *const module, QWidget *const parent, const int index) override;

private:
    ListView *m_view;
    ModuleDataModel *m_model;
};
DCC_END_NAMESPACE
#endif // MAINLAYOUT_H
