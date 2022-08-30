// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef GROUPITEM_H
#define GROUPITEM_H

#include "interface/namespace.h"

#include <DStyleOption>
#include <DStyleHelper>
#include <DListView>

namespace DCC_NAMESPACE {
namespace accounts {
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
}
#endif // GROUPITEM_H
