#ifndef DISPLAYITEMDELEGATE_H
#define DISPLAYITEMDELEGATE_H

#include <QAbstractItemDelegate>

class DisplayItemDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:
    explicit DisplayItemDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // DISPLAYITEMDELEGATE_H
