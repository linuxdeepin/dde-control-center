#include "navgationview.h"

namespace dcc {

NavgationView::NavgationView(QWidget *parent)
    : QListView(parent)
{
    setViewMode(QListView::IconMode);
    setFrameStyle(QFrame::NoFrame);
    setSpacing(1);
//    setStyleSheet("margin: 0 0 0 13px; background: transparent;");
}

}
