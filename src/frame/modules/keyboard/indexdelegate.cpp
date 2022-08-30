// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "indexdelegate.h"
#include "indexmodel.h"
#include "widgets/basiclistdelegate.h"

#include <QPainter>

namespace dcc {
namespace keyboard{

IndexDelegate::IndexDelegate(QObject *parent)
    :QItemDelegate(parent)
{

}

QSize IndexDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant value = index.data(Qt::SizeHintRole);

    if(value.isValid())
        return qvariant_cast<QSize>(value);

    QSize size = QItemDelegate::sizeHint(option, index);
    QVariant var = index.data();
    MetaData md = var.value<MetaData>();
    if(md.section())
        size.setHeight(26);
    else
        size.setHeight(36);
    return size;
}

void IndexDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant var = index.data();
    MetaData md = var.value<MetaData>();

    QPen pen = painter->pen();

    painter->setRenderHints(painter->renderHints() | QPainter::Antialiasing);

    painter->setPen(Qt::NoPen);
    if (md.section()) {
        painter->setOpacity(0.2);
        painter->setBrush(QColor(222, 222, 222));
        painter->drawRect(option.rect.adjusted(0, 0, 0, -1));
    }

    painter->setOpacity(1);
    painter->setPen(pen);


    painter->drawText(option.rect.adjusted(20, 0, 0, 0), Qt::AlignVCenter, md.text());
}
}
}
