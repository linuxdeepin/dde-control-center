// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"

#include <QMetaType>
#include <QStyledItemDelegate>

struct LastItemData
{
    bool isDrawLast;
    QString iconPath;
};
Q_DECLARE_METATYPE(LastItemData)

QT_BEGIN_NAMESPACE
class QObject;
class QStyleOptionViewItem;
class QModelIndex;
class QPainter;
class QSize;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {

class AvatarItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit AvatarItemDelegate(bool isCustom, QObject *parent = nullptr);

    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    // set item size
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    bool m_isCustom;
};
}
 // namespace DCC_NAMESPACE
