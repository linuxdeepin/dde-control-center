#ifndef VPNLISTVIEW_H
#define VPNLISTVIEW_H

#include <QListView>

class VpnListView : public QListView
{
    Q_OBJECT

public:
    explicit VpnListView(QWidget *parent = 0);

    QSize sizeHint() const;
};

#endif // VPNLISTVIEW_H
