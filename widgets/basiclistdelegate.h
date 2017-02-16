#ifndef BASICLISTDELEGATE_H
#define BASICLISTDELEGATE_H

#include <QAbstractItemDelegate>

namespace dcc {

namespace widgets {

class BasicListDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:
    explicit BasicListDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

}

}

#endif // BASICLISTDELEGATE_H
