#ifndef SIDEBARDELEGATE_H
#define SIDEBARDELEGATE_H

#include <QItemDelegate>

class SidebarDelegate : public QItemDelegate
{
public:
    SidebarDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // SIDEBARDELEGATE_H
