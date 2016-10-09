#ifndef FRAMEWIDGET_H
#define FRAMEWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>

///
/// FrameWidget can auto adjust size to fit parent Frame
///

class Frame;
class FrameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FrameWidget(Frame *parent = 0);

    QWidget *setContent(QWidget * const c);

    void show();
    void hide();
    void showBack();
    void destory();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void destorySelf();

private:
    QPropertyAnimation *m_slideAni;

    QWidget *m_content;
};

#endif // FRAMEWIDGET_H
