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
#ifdef QT_DEBUG
    setStyleSheet("background-color: rgba(238, 238, 238, .238);");
#endif
}

void NavgationView::leaveEvent(QEvent *e)
{
    QListView::leaveEvent(e);

    emit entered(QModelIndex());
}

}
