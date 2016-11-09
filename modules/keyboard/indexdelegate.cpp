#include "indexdelegate.h"
#include "indexmodel.h"
#include <QPainter>

IndexDelegate::IndexDelegate(QObject *parent)
    :QItemDelegate(parent)
{

}

QSize IndexDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant value = index.data(Qt::SizeHintRole);

    if(value.isValid())
        return qvariant_cast<QSize>(value);

    QSize size = QItemDelegate::sizeHint(option, index);
    QVariant var = index.data();
    MetaData md = var.value<MetaData>();
    if(md.section())
        size.setHeight(26);
    else
        size.setHeight(36);
    return size;
}

void IndexDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QItemDelegate::paint(painter, option, index);
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());

    QVariant var = index.data();
    MetaData md = var.value<MetaData>();
    if(md.section())
    {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        QPen pen = painter->pen();
        painter->setOpacity(0.5);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(238, 238, 238));
        painter->drawRect(option.rect.adjusted(0,1,0,-1));
        painter->setPen(pen);
        painter->restore();
        painter->drawText(option.rect.adjusted(3,0,0,0), Qt::AlignVCenter, md.text());
    }
    else
    {
        painter->save();
        painter->setOpacity(0.2);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(238, 238, 238));
        painter->drawRect(option.rect.adjusted(0,1,0,-1));
        painter->restore();
        painter->drawText(option.rect.adjusted(6,0,0,0), Qt::AlignVCenter, md.text());
    }
}
