#include "navgationdelegate.h"

#include <QPainter>

namespace dcc {

NavgationDelegate::NavgationDelegate(QObject *parent)
    : QItemDelegate(parent)
{

}

void NavgationDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(QPainter::Antialiasing);

    painter->fillRect(option.rect, Qt::red);

    const QString txt = index.data().toString();
    painter->drawText(option.rect, txt);
}

QSize NavgationDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}

}
