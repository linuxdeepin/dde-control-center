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
//    setStyleSheet("background-color: red;");
}

}
