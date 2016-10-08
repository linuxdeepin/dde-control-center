
#include "frame.h"
#include "framewidget.h"

#include <QResizeEvent>

FrameWidget::FrameWidget(Frame *parent)
    : QWidget(parent)
{
    parent->installEventFilter(this);
    setFixedSize(parent->size());
}

bool FrameWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parent() && event->type() == QEvent::Resize)
        setFixedSize(static_cast<QResizeEvent *>(event)->size());

    return false;
}
