#ifndef NAVGATIONDELEGATE_H
#define NAVGATIONDELEGATE_H

#include <QItemDelegate>

namespace dcc {

class NavgationDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit NavgationDelegate(QObject *parent = 0);

private:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

}

#endif // NAVGATIONDELEGATE_H
