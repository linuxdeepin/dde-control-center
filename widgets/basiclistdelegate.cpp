#include "basiclistdelegate.h"
#include "basiclistmodel.h"

#include <QPainter>

namespace dcc {

namespace widgets {

BasicListDelegate::BasicListDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{

}

void BasicListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter->setPen(Qt::black);
    painter->setBrush(Qt::red);

    painter->drawText(option.rect.marginsRemoved(QMargins(25, 0, 0, 0)), index.data(Qt::DisplayRole).toString());

    if (index.data(BasicListModel::ItemSelectedRole).toBool())
    {
        const int x = option.rect.right() - 16 - 10;
        const int y = option.rect.top() + (option.rect.height() - 16) / 2;

        painter->drawPixmap(x, y, QPixmap(":/widgets/themes/dark/icons/select.png"));
    }
}

QSize BasicListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}

}

}
