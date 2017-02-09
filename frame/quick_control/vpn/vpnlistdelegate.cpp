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
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter->setPen(Qt::white);
    painter->setBrush(Qt::red);

    if (index.data(VpnListModel::VpnItemHoveredRole).toBool())
        painter->fillRect(option.rect, QColor(0, 0, 0, .6 * 255));

    painter->drawText(option.rect.marginsRemoved(QMargins(10, 0, 0, 0)), index.data(Qt::DisplayRole).toString(), Qt::AlignLeft | Qt::AlignVCenter);

    if (index.data(VpnListModel::VpnShowIconRole).toBool())
    {
        const QPixmap pixmap = index.data(VpnListModel::VpnIconRole).value<QPixmap>();

        const int l = 16;
        const int x = option.rect.right() - l - 10;
        const int y = option.rect.top() + (option.rect.height() - l) / 2;

        painter->drawPixmap(x, y, pixmap);
    }
}

QSize VpnListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}
