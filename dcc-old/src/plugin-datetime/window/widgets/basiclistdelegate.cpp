//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "basiclistdelegate.h"
#include "basiclistmodel.h"

#include <QPainter>
#include <QPainterPath>

QPixmap loadPixmap(const QString &path)
{
    qreal ratio = 1.0;
    QPixmap pixmap;

    const qreal devicePixelRatio = qApp->devicePixelRatio();

    if (!qFuzzyCompare(ratio, devicePixelRatio)) {
        QImageReader reader;
        reader.setFileName(qt_findAtNxFile(path, devicePixelRatio, &ratio));
        if (reader.canRead()) {
            reader.setScaledSize(reader.size() * (devicePixelRatio / ratio));
            pixmap = QPixmap::fromImage(reader.read());
            pixmap.setDevicePixelRatio(devicePixelRatio);
        }
    } else {
        pixmap.load(path);
    }

    return pixmap;
}


BasicListDelegate::BasicListDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{

}

void BasicListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const bool isHover = index.data(BasicListModel::ItemHoverRole).toBool();

    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter->setBrush(Qt::red);
    painter->setPen(Qt::black);

    if (isHover) {
        QPainterPath path;
        path.addRoundedRect(option.rect.marginsRemoved(QMargins(15, 0, 5, 0)), 6, 6);
        painter->fillPath(path, QColor(0, 0, 0, 0.05 * 255));
    }

    painter->drawText(option.rect.marginsRemoved(QMargins(30, 0, 0, 0)), Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());

    if (index.data(BasicListModel::ItemSelectedRole).toBool())
    {
        const int x = option.rect.right() - 16 - 14;
        const int y = option.rect.top() + (option.rect.height() - 16) / 2;

        painter->drawPixmap(x, y, loadPixmap(":/widgets/themes/dark/icons/list_select.png"));
    }
}

QSize BasicListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}

