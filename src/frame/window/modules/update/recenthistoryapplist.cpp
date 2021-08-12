/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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

#include "recenthistoryapplist.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::update;

RecentHistoryApplist::RecentHistoryApplist(QWidget *parent)
    : ContentWidget(parent)
{
    setAccessibleName("RecentHistoryApplist");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void RecentHistoryApplist::setContentWidget(QWidget *widget)
{
    setContent(widget);
}
