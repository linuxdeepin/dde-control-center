#include "wifilistdelegate.h"
#include "wifilistmodel.h"

#include <QPainter>

WifiListDelegate::WifiListDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{

}

void WifiListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter->setPen(Qt::white);
    painter->setBrush(Qt::red);

    if (index.data(WifiListModel::ItemHoveredRole).toBool())
        painter->fillRect(option.rect, QColor(0, 0, 0, .6 * 255));

    if (index.data(WifiListModel::ItemIsHeaderRole).toBool())
        painter->setPen(Qt::red);
    else
        painter->setPen(Qt::white);

    painter->drawText(option.rect, Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());
}

QSize WifiListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}
