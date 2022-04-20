/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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
#ifndef GROUPITEM_H
#define GROUPITEM_H

#include "interface/namespace.h"

#include <DStyleOption>
#include <DStyleHelper>
#include <DListView>

namespace DCC_NAMESPACE {
class GroupItem : public DTK_WIDGET_NAMESPACE::DStandardItem
{
public:
    using DStandardItem::DStandardItem;
    bool operator<(const QStandardItem &other) const override {
        if ( checkState() != other.checkState()) {
            return checkState() > other.checkState();
        }
        return text().toLower() < other.text().toLower();
    }
};
}
#endif // GROUPITEM_H
