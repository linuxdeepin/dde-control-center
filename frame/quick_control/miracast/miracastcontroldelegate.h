#ifndef MIRACASTCONTROLDELEGATE_H
#define MIRACASTCONTROLDELEGATE_H

#include <QAbstractItemDelegate>

class miracastControlDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:
    explicit miracastControlDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // MIRACASTCONTROLDELEGATE_H
