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
#ifndef PAGELAYOUT_H
#define PAGELAYOUT_H
#include "interface/layoutbase.h"
class QVBoxLayout;
class QHBoxLayout;
class QScrollArea;
DCC_BEGIN_NAMESPACE
class PageLayout : public LayoutBase
{
public:
    PageLayout();

    void setCurrent(DCC_NAMESPACE::ModuleObject *const child) override;
    QWidget *layoutModule(DCC_NAMESPACE::ModuleObject *const module, QWidget *const parent, const QList<ModuleObject *> &children) override;

private:
    QWidget *getPage(QWidget *const widget, const QString &title);
    void removeChild(DCC_NAMESPACE::ModuleObject *const childModule);
    void addChild(DCC_NAMESPACE::ModuleObject *const childModule);

private:
    QVBoxLayout *m_vlayout; // 上方纵向布局
    QHBoxLayout *m_hlayout; // 底下横向布局
    QMap<ModuleObject *, QWidget *> m_mapWidget;
    QScrollArea *m_area;
    DCC_NAMESPACE::ModuleObject *m_module;
};
DCC_END_NAMESPACE
#endif // PAGELAYOUT_H
