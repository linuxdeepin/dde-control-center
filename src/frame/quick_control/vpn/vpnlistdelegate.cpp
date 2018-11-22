/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "vpnlistdelegate.h"
#include "vpnlistmodel.h"
#include "widgets/basiclistdelegate.h"

#include <QPainter>
#include <QDebug>
#include <QDateTime>

VpnListDelegate::VpnListDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{

}

void VpnListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter->setBrush(Qt::red);

    const bool isHover = index.data(VpnListModel::VpnItemHoveredRole).toBool();
    const bool isNext = index.data(VpnListModel::VpnNextRole).toBool();
    const bool isDisable = index.data(VpnListModel::VpnDisableRole).toBool();

    if (isHover)
        painter->fillRect(option.rect, QColor(255, 255, 255, 0.1 * 255));

    painter->setPen(Qt::white);

    if (isDisable)
        painter->drawText(option.rect, Qt::AlignCenter, index.data(Qt::DisplayRole).toString());
    else
        painter->drawText(option.rect.marginsRemoved(QMargins(30, 0, 0, 0)), index.data(Qt::DisplayRole).toString(), Qt::AlignLeft | Qt::AlignVCenter);

    if (index.row())
    {
        if (!isNext && !isHover) {
            painter->setPen(QColor(255, 255, 255, 255 * 0.05));
            painter->drawLine(QPoint(10, option.rect.top()), QPoint(option.rect.right() - 10, option.rect.top()));
        }
    }
    else
    {
        painter->setPen(QColor(255, 255, 255, 255 * 0.1));
        painter->drawLine(QPoint(0, option.rect.top()), QPoint(option.rect.right(), option.rect.top()));
    }

    // draw icon
    const int l = 16;
    const int x = option.rect.right() - l - 25;
    const int y = option.rect.top() + (option.rect.height() - l) / 2;

    const VpnListModel::VpnState state = index.data(VpnListModel::VpnStateRole).value<VpnListModel::VpnState>();
    if (state == VpnListModel::Activing)
    {
        const quint64 index = QDateTime::currentMSecsSinceEpoch() / 20;
        const QString pix = QString(":/frame/themes/light/icons/white_loading/loading_0%1.png").arg((index % 90), 2, 10, QChar('0'));

        painter->drawPixmap(x, y, loadPixmap(pix));
    } else if (state == VpnListModel::Actived) {
        const QPixmap pixmap = index.data(VpnListModel::VpnIconRole).value<QPixmap>();

        painter->drawPixmap(x, y, pixmap);
    }
}

QSize VpnListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}
