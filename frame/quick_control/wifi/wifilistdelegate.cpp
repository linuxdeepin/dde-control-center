#include "wifilistdelegate.h"
#include "wifilistmodel.h"

#include <QPainter>
#include <QDebug>

WifiListDelegate::WifiListDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{

}

void WifiListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter->setPen(Qt::white);
    painter->setBrush(Qt::red);

    const bool isHeader = index.data(WifiListModel::ItemIsHeaderRole).toBool();
    const bool isHovered = index.data(WifiListModel::ItemHoveredRole).toBool();

    if (isHovered && !isHeader)
        painter->fillRect(option.rect, QColor(0, 0, 0, .6 * 255));

    if (isHeader)
        painter->fillRect(option.rect, QColor(255, 255, 255, .3 * 255));

    if (isHeader)
        painter->setPen(Qt::red);
    else
        painter->setPen(Qt::white);

    if (isHeader)
        painter->drawText(option.rect.marginsRemoved(QMargins(10, 0, 0, 0)), Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());
    else
        painter->drawText(option.rect.marginsRemoved(QMargins(20, 0, 0, 0)), Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());
}

QSize WifiListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}
