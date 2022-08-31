// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"

#include <QStyledItemDelegate>
#include <QMetaType>

struct LastItemData {
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
namespace accounts {

class AvatarItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit AvatarItemDelegate(QObject *parent = nullptr);

    // painting
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    // set item size
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

}
}
