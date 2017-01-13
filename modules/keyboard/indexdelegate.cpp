#include "indexdelegate.h"
#include "indexmodel.h"
#include <QPainter>

namespace dcc {
namespace keyboard{

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

    QVariant var = index.data();
    MetaData md = var.value<MetaData>();

    QPen pen = painter->pen();

    painter->setRenderHints(painter->renderHints() | QPainter::Antialiasing);

    painter->setOpacity(0.2);
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());
    else {
        painter->setPen(Qt::NoPen);
        painter->setBrush(md.section() ? QColor(222, 222, 222) : QColor(238, 238, 238));
        painter->drawRect(option.rect.adjusted(0, 0, 0, -1));
    }

    painter->setOpacity(1);
    painter->setPen(pen);
    painter->drawText(option.rect.adjusted(20, 0, 0, 0), Qt::AlignVCenter, md.text());
}
}
}
