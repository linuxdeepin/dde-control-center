#include "vpnlistdelegate.h"
#include "vpnlistmodel.h"

#include <QPainter>
#include <QDebug>

VpnListDelegate::VpnListDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{

}

void VpnListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter->setBrush(Qt::red);

    if (index.data(VpnListModel::VpnItemHoveredRole).toBool())
        painter->fillRect(option.rect, QColor(0, 0, 0, .6 * 255));

    if (index.row())
    {
        painter->setPen(QColor(255, 255, 255, 255 * .3));
        painter->drawLine(QPoint(10, option.rect.top()), QPoint(option.rect.right() - 10, option.rect.top()));
    }
    else
    {
        painter->setPen(Qt::white);
        painter->drawLine(QPoint(0, option.rect.top()), QPoint(option.rect.right(), option.rect.top()));
    }

    painter->setPen(Qt::white);
    painter->drawText(option.rect.marginsRemoved(QMargins(30, 0, 0, 0)), index.data(Qt::DisplayRole).toString(), Qt::AlignLeft | Qt::AlignVCenter);

    if (index.data(VpnListModel::VpnShowIconRole).toBool())
    {
        const QPixmap pixmap = index.data(VpnListModel::VpnIconRole).value<QPixmap>();

        const int l = 16;
        const int x = option.rect.right() - l - 25;
        const int y = option.rect.top() + (option.rect.height() - l) / 2;

        painter->drawPixmap(x, y, pixmap);
    }
}

QSize VpnListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}
