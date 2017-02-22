#include "modulewidgetheader.h"

#include <QPainter>

namespace dcc {

namespace widgets {

ModuleWidgetHeader::ModuleWidgetHeader(QWidget *parent)
    : QFrame(parent)
{
    setStyleSheet("QFrame:hover {"
                  "background-color: red;"
                  "}");
}

void ModuleWidgetHeader::mouseReleaseEvent(QMouseEvent *e)
{
    QFrame::mouseReleaseEvent(e);

    emit clicked();
}

}

}
