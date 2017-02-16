#include "basiclistview.h"

#include <QDebug>

namespace dcc {

namespace widgets {

BasicListView::BasicListView(QWidget *parent)
    : QListView(parent)
{
//    setStyleSheet("background-color: red;");
    setFrameStyle(QFrame::NoFrame);
    setMouseTracking(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollMode(ScrollPerPixel);
    setContentsMargins(0, 0, 0, 0);
    setSpacing(0);
    setSelectionMode(NoSelection);
}

QSize BasicListView::sizeHint() const
{
    int height = 0;
    const int count = model()->rowCount();
    for (int i(0); i != count; ++i)
        height += sizeHintForRow(i);

    return QSize(QListView::sizeHint().width(), height);
}

void BasicListView::setModel(QAbstractItemModel *model)
{
    QListView::setModel(model);

    setMaximumHeight(sizeHint().height());
    connect(model, &QAbstractItemModel::layoutChanged, this, [=] { setMaximumHeight(sizeHint().height()); }, Qt::UniqueConnection);
}

void BasicListView::leaveEvent(QEvent *e)
{
    QListView::leaveEvent(e);

    emit entered(QModelIndex());
}

}

}
