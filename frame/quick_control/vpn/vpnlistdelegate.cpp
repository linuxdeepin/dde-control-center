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
