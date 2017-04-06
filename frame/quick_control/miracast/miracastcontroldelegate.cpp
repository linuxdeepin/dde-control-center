#include "miracastcontroldelegate.h"
#include "miracastcontrolmodel.h"

#include <QPainter>

miracastControlDelegate::miracastControlDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{

}

void miracastControlDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const MiracastInfo info = index.data(MiracastControlModel::MiracastItemInfoRole).value<MiracastInfo>();

    painter->setRenderHints(QPainter::Antialiasing);

    // is header
    if (!info.m_sink && info.m_link)
        painter->setPen(Qt::white);
    else
        painter->setPen(Qt::yellow);

    const QRect r = option.rect.marginsRemoved(QMargins(10, 2, 10, 2));

    painter->drawText(r, Qt::AlignLeft | Qt::AlignVCenter, index.data(MiracastControlModel::MiracastDisplayRole).toString());
    if (info.m_sink)
        painter->drawText(r, Qt::AlignRight | Qt::AlignVCenter, info.m_sink->m_connected ? tr("Connected") : tr("Unconnected"));
    else
        painter->drawText(r, Qt::AlignRight | Qt::AlignVCenter, info.m_link->m_managed ? tr("Actived") : tr("Inactive"));
}

QSize miracastControlDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}
