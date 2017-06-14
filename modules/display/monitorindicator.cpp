#include "monitorindicator.h"

#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QX11Info>

#include <X11/extensions/shape.h>
#include <X11/Xregion.h>

using namespace dcc;
using namespace dcc::display;

MonitorIndicator::MonitorIndicator(QWidget *parent)
    : QFrame(parent)
{
    setWindowFlags(Qt::SplashScreen | Qt::X11BypassWindowManagerHint);
    setStyleSheet("background-color: #2ca7f8;");
}

void MonitorIndicator::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);

    XRectangle rectangle;
    rectangle.x = 0;
    rectangle.y = 0;
    rectangle.width = e->size().width();
    rectangle.height = e->size().height();

    // need to restore the cut area, if not,cut out will be repeated.
    XShapeCombineRectangles(QX11Info::display(), winId(), ShapeBounding, 0, 0, &rectangle, 1, ShapeSet, YXBanded);

    rectangle.x = 10;
    rectangle.y = 10;
    rectangle.width = e->size().width() - 20;
    rectangle.height = e->size().height() - 20;

    XShapeCombineRectangles(QX11Info::display(), winId(), ShapeBounding, 0, 0, &rectangle, 1, ShapeSubtract, YXBanded);
}
