#include "navdelegate.h"

#include <QPainter>

NavDelegate::NavDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void NavDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    QRect rect = QRect(option.rect.left(), option.rect.top(), option.rect.width()-1, option.rect.height()-1);
    if (option.state & QStyle::State_MouseOver) {
        painter->fillRect(rect, QColor(255, 255, 255, 25.5));
    } else {
        painter->fillRect(rect, QColor(255, 255, 255, 7.65));
    }

    QString moduleName = index.data(Qt::WhatsThisRole).toString();
    if (!moduleName.isEmpty()) {
        QPixmap modulePm = QPixmap(QString(":/icons/nav_%1.png").arg(moduleName));
        painter->drawPixmap(option.rect.center().x() - modulePm.width()/2,
                            option.rect.center().y() - modulePm.height()/2,
                            modulePm);
    }
}
