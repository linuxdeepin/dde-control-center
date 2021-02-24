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

#include "groupview.h"

#include <DListView>

#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;
using namespace DCC_NAMESPACE::personalization;

GroupView::GroupView(QWidget *parent)
    : SettingsItem(parent)
    , m_listView(new DListView())
{
    QVBoxLayout *m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 18, 20, 0);
    m_mainLayout->addWidget(m_listView);
}
