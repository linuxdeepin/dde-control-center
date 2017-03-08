#include "navgationview.h"

#include <QPainter>

namespace dcc {

NavgationView::NavgationView(QWidget *parent)
    : QListView(parent)
{
    setViewMode(QListView::IconMode);
    setFrameStyle(QFrame::NoFrame);
    setSpacing(1);
    setMouseTracking(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //    setStyleSheet("background-color: red; border-radius: 5px;");
}

void NavgationView::leaveEvent(QEvent *e)
{
    QListView::leaveEvent(e);

    emit entered(QModelIndex());
}

}
