/*
   * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
   *
   * Author:     xiechuan <xiechuan@uniontech.com>
   *
   * Maintainer: xiechaun <xiechaun@uniontech.com>
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

#ifndef GROUPVIEW_H
#define GROUPVIEW_H

#include "widgets/settingsitem.h"
#include "interface/namespace.h"

DWIDGET_BEGIN_NAMESPACE
class DListView;
DWIDGET_END_NAMESPACE

using namespace dcc::widgets;

namespace DCC_NAMESPACE {
namespace personalization {
class GroupView : public SettingsItem
{
    Q_OBJECT
public:
    explicit GroupView(QWidget *parent = nullptr);
    DTK_WIDGET_NAMESPACE::DListView *getListView() { return m_listView; }

private:
    DTK_WIDGET_NAMESPACE::DListView *m_listView;
};
} // namespace personalization
} // namespace DCC_NAMESPACE

#endif // GROUPVIEW_H
