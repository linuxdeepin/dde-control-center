#include "vpnlistview.h"

#include <QDebug>

VpnListView::VpnListView(QWidget *parent)
    : QListView(parent)
{
//    setStyleSheet("background-color: red;");
    setFrameStyle(QFrame::NoFrame);
}

QSize VpnListView::sizeHint() const
{
    return QSize(QListView::sizeHint().width(), sizeHintForRow(0) * model()->rowCount());
}
