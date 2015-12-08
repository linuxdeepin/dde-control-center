#include "sidebardelegate.h"
#include "sidebarmodel.h"

#include <QPainter>
#include <QDebug>

SidebarDelegate::SidebarDelegate(QObject *parent)
    : QItemDelegate(parent)
{

}

void SidebarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QPixmap pixmap;
    if (option.state & QStyle::State_MouseOver)
        pixmap = index.data(SidebarModel::HoverIcon).value<QPixmap>();
    else
        pixmap = index.data(SidebarModel::DefaultIcon).value<QPixmap>();

    painter->drawPixmap(option.rect.center() - pixmap.rect().center(), pixmap);
}

