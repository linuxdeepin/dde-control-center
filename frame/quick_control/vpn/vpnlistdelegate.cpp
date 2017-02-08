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
    painter->drawText(option.rect, index.data(Qt::DisplayRole).toString(), Qt::AlignLeft | Qt::AlignVCenter);

    if (index.data(VpnListModel::VpnConnectedRole).toBool())
    {
        QRect r = option.rect;
        r.setLeft(r.right() - 20 - 10);
        r.setRight(r.right() - 10);
        r.setHeight(20);
        r.moveTop(option.rect.top() + (option.rect.height() - 20) / 2);
        painter->fillRect(r, Qt::red);
    }
}

QSize VpnListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}
