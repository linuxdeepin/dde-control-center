#ifndef FRAMEWIDGET_H
#define FRAMEWIDGET_H

#include <QWidget>

///
/// FrameWidget can auto adjust size to fit parent Frame
///

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
