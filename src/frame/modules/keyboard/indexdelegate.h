// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef INDEXDELEGATE_H
#define INDEXDELEGATE_H

#include <QItemDelegate>

namespace dcc {
namespace keyboard{

class IndexDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit IndexDelegate(QObject *parent = 0);

protected:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

}
}
#endif // INDEXDELEGATE_H
