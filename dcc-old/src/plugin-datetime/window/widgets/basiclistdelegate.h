//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef BASICLISTDELEGATE_H
#define BASICLISTDELEGATE_H

#include <QAbstractItemDelegate>
#include <QIcon>
#include <QApplication>
#include <QImageReader>

QPixmap loadPixmap(const QString &path);


class BasicListDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:
    explicit BasicListDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};


#endif // BASICLISTDELEGATE_H
