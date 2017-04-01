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

    painter->drawText(option.rect, Qt::AlignLeft | Qt::AlignVCenter, index.data(MiracastControlModel::MiracastDisplayRole).toString());
    if (info.m_sink)
        painter->drawText(option.rect, Qt::AlignRight | Qt::AlignVCenter, info.m_sink->m_connected ? tr("Connected") : tr("Disconnect"));
    else
        painter->drawText(option.rect, Qt::AlignRight | Qt::AlignVCenter, info.m_link->m_managed ? tr("Active") : tr("Inactive"));
}

QSize miracastControlDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}
