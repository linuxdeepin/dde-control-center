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
#ifndef VLISTLAYOUT_H
#define VLISTLAYOUT_H
#include "layoutbase.h"

#include <DListView>
DCC_BEGIN_NAMESPACE
class ModuleDataModel;
class VListLayout : public LayoutBase
{
public:
    VListLayout();

    int layoutType() const override;
    void setCurrent(DCC_NAMESPACE::ModuleObject *const child) override;
    QWidget *layoutModule(DCC_NAMESPACE::ModuleObject *const module, QWidget *const parent, const int index) override;

private:
    DTK_WIDGET_NAMESPACE::DListView *m_view;
    ModuleDataModel *m_model;
};
DCC_END_NAMESPACE
#endif // VLISTLAYOUT_H
