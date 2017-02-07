#ifndef VPNLISTVIEW_H
#define VPNLISTVIEW_H

#include <QListView>
#include <QAbstractItemModel>

class BasicListView : public QListView
{
    Q_OBJECT

public:
    explicit BasicListView(QWidget *parent = 0);

    QSize sizeHint() const;

public:
    void setModel(QAbstractItemModel *model);
};

#endif // VPNLISTVIEW_H
