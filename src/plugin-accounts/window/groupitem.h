//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
