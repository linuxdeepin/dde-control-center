#ifndef VPNLISTVIEW_H
#define VPNLISTVIEW_H

#include <QListView>
#include <QAbstractItemModel>

namespace dcc {

namespace widgets {

class BasicListView : public QListView
{
    Q_OBJECT

public:
    explicit BasicListView(QWidget *parent = 0);

    QSize sizeHint() const;

public:
    void setModel(QAbstractItemModel *model);
    void onContentHeightChanged();

    void leaveEvent(QEvent *e);

public slots:
    void setAutoFitHeight(const bool fit);

private:
    bool m_autoFitHeight;
};

}

}

#endif // VPNLISTVIEW_H
