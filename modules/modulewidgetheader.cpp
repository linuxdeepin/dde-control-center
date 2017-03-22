#include "modulewidgetheader.h"

#include <QPainter>
#include <QMouseEvent>

namespace dcc {

namespace widgets {

ModuleWidgetHeader::ModuleWidgetHeader(QWidget *parent)
    : QFrame(parent)
{
    setCursor(Qt::PointingHandCursor);
    setStyleSheet("dcc--widgets--ModuleWidgetHeader {"
                  "padding: 5px 0;"
                  "}"
                  "dcc--widgets--ModuleWidgetHeader:hover {"
                  "background-color: rgba(255, 255, 255, 0.1);"
                  "border-radius: 4px;"
                  "}");
}

void ModuleWidgetHeader::mouseReleaseEvent(QMouseEvent *e)
{
    QFrame::mouseReleaseEvent(e);

    if (e->button() != Qt::LeftButton)
        return;

    e->accept();

    emit clicked();
}

}

}
