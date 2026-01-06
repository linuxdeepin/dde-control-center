// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCQUICKREPEATER_H
#define DCCQUICKREPEATER_H

#include "private/qquickrepeater_p.h"

namespace dccV25 {
class DccQuickRepeater : public QQuickRepeater
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Repeater)
public:
    explicit DccQuickRepeater(QQuickItem *parent = nullptr);
    ~DccQuickRepeater() override;

protected:
    void onItemAdded(int index, QQuickItem *item);
};
} // namespace dccV25
#endif // DCCQUICKREPEATER_H
