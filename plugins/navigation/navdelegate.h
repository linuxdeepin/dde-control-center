#ifndef NAVDELEGATE_H
#define NAVDELEGATE_H

#include <QStyledItemDelegate>

class NavDelegate : public QStyledItemDelegate
{
public:
    NavDelegate(QObject *parent = 0);

public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // NAVDELEGATE_H
