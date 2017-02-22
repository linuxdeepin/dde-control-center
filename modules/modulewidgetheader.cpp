#include "modulewidgetheader.h"

#include <QPainter>

namespace dcc {

namespace widgets {

ModuleWidgetHeader::ModuleWidgetHeader(QWidget *parent)
    : QFrame(parent)
{
    setCursor(Qt::PointingHandCursor);
    setStyleSheet("QFrame:hover {"
//                  "background-color: rgba(255, 255, 255, 0.2);"
                  "}");
}

void ModuleWidgetHeader::mouseReleaseEvent(QMouseEvent *e)
{
    QFrame::mouseReleaseEvent(e);

    emit clicked();
}

}

}
