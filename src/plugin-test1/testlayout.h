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
#ifndef TESTLAYOUT_H
#define TESTLAYOUT_H
#include "interface/layoutbase.h"
// 定义布局的key，在使用自定义布局时会用到，注册布局时，如果key已被使用会对应报错，并注册失败
// key通常用预设的DCC_LAYOUT_TYPE或上一个数字，在使用时没找到该布局则会根据key值使用一个接近的预设DCC_LAYOUT_TYPE，如果没接近的则使用ModuleObject::Page
#define TESTLAYOUT_KEY (DCC_NAMESPACE::ModuleObject::Page | 1)
class QFormLayout;
class QScrollArea;
class testLayout : public DCC_NAMESPACE::LayoutBase
{
public:
    testLayout();

    virtual void setCurrent(DCC_NAMESPACE::ModuleObject *const child);
    virtual QWidget *layoutModule(DCC_NAMESPACE::ModuleObject *const module, QWidget *const parent, DCC_NAMESPACE::ModuleObject *const child);

private:
    void removeChild(DCC_NAMESPACE::ModuleObject *const childModule);
    void addChild(DCC_NAMESPACE::ModuleObject *const childModule);

private:
    QFormLayout *m_layout;
    QList<DCC_NAMESPACE::ModuleObject *> m_modules;
    DCC_NAMESPACE::ModuleObject *m_module;
    QScrollArea *m_area;
};

#endif // TESTLAYOUT_H
