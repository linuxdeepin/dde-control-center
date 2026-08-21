// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccquickrepeater.h"

namespace dccV25 {
DccQuickRepeater::DccQuickRepeater(QQuickItem *parent)
    : QQuickRepeater(parent)
{
    connect(this, &QQuickRepeater::itemAdded, this, &DccQuickRepeater::updateItemParent);
}

DccQuickRepeater::~DccQuickRepeater() { }

void DccQuickRepeater::updateItemParent([[maybe_unused]] int index, QQuickItem *item)
{
    if (item && !item->parent()) {
        item->setParent(this);
    }
}
} // namespace dccV25
