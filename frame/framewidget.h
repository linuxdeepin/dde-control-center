#ifndef FRAMEWIDGET_H
#define FRAMEWIDGET_H

#include <QWidget>

class Frame;
class FrameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FrameWidget(Frame *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // FRAMEWIDGET_H
