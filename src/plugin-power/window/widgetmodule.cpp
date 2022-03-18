/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
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
#include "widgetmodule.h"

WidgetModuleBase::WidgetModuleBase(const QString &name, const QString &displayName, QObject *parent)
    : DCC_NAMESPACE::ModuleObject(name, QStringList({ displayName }), parent)
    , m_widget(nullptr)
{
}

void WidgetModuleBase::onUpdateWidget(QWidget *widget)
{
    emit updateWidget(widget);
}
