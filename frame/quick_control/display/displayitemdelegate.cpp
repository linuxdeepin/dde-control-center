#include "displayitemdelegate.h"
#include "displaycontrolmodel.h"

#include <QPainter>

DisplayItemDelegate::DisplayItemDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{

}

void DisplayItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    QPen p(Qt::white);
    p.setWidth(1);
    painter->setPen(p);

    if (!index.data(DisplayControlModel::ItemIsLastRole).toBool())
        painter->drawLine(QPoint(60, option.rect.bottom()), QPoint(option.rect.right() - 30, option.rect.bottom()));

    const int icon_x = 20;
    const int icon_y = option.rect.top() + (option.rect.height() - 30) / 2;
    painter->fillRect(icon_x, icon_y, 30, 30, Qt::red);

    if (index.data(DisplayControlModel::ItemSelectedRole).toBool())
    {
        const int x = option.rect.right() - 24;
        const int y = option.rect.top() + (option.rect.height() - 16) / 2;

        painter->drawPixmap(x, y, QPixmap(":/frame/themes/dark/icons/select.png"));
    }

    const int name_x = 65;
    const int name_y = option.rect.top() + 20;
    painter->drawText(name_x, name_y, index.data(DisplayControlModel::ItemTitleRole).toString());

    QRect descRect = option.rect;
    descRect.setTopLeft(QPoint(name_x, name_y + 5));
    descRect.setRight(option.rect.right() - 30);
    painter->setPen(QColor(255, 255, 255, .6 * 255));
    painter->drawText(descRect, Qt::AlignLeft | Qt::TextWordWrap, index.data(DisplayControlModel::ItemDescriptionRole).toString());
}

QSize DisplayItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    return index.data(Qt::SizeHintRole).toSize();
}
