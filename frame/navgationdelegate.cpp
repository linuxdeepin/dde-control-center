#include "navgationdelegate.h"
#include "navgationmodel.h"

#include <QPainter>

namespace dcc {

NavgationDelegate::NavgationDelegate(QObject *parent)
    : QItemDelegate(parent)
{

}

void NavgationDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(QPainter::Antialiasing);

    if (index.data(NavgationModel::ItemHoveredRole).toBool())
        painter->fillRect(option.rect, QColor::fromRgbF(1, 1, 1, 0.3));
    else
        painter->fillRect(option.rect, QColor::fromRgbF(1, 1, 1, 0.2));

    const QString module = index.data().toString();
    if (module.isEmpty())
        return;

    const QString picFile = QString(":/%1/themes/dark/icons/nav_%2.png").arg(module).arg(module);
    QPixmap pix = QPixmap(picFile);

    QRect pixRect = pix.rect();
    pixRect.moveCenter(option.rect.center());

    painter->drawPixmap(pixRect, pix);
}

QSize NavgationDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}

}
