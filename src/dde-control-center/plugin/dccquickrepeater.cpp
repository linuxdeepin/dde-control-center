// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccquickrepeater.h"

namespace dccV25 {
DccQuickRepeater::DccQuickRepeater(QQuickItem *parent)
    : QQuickRepeater(parent)
{
    connect(this, &QQuickRepeater::itemAdded, this, &DccQuickRepeater::onItemAdded);
}

DccQuickRepeater::~DccQuickRepeater() { }

void DccQuickRepeater::onItemAdded(int, QQuickItem *item)
{
    if (item && !item->parent()) {
        item->setParent(this);
    }
}
} // namespace dccV25
