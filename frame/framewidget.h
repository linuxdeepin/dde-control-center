#ifndef FRAMEWIDGET_H
#define FRAMEWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>

#include "translucentframe.h"

///
/// FrameWidget can auto adjust size to fit parent Frame
///

using namespace dcc::widgets;

namespace dcc {

class ContentWidget;

}

class Frame;
class FrameWidget : public TranslucentFrame
{
    Q_OBJECT

public:
    explicit FrameWidget(Frame *parent = 0);

    dcc::ContentWidget *setContent(dcc::ContentWidget * const c);
    dcc::ContentWidget *content();

    void show();
    void hide();
    void showBack();
    void hideBack();
    void destory();

    int animationDuration() const;

signals:
    void contentDetached(dcc::ContentWidget * const w) const;

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void destorySelf();

private:
    QPropertyAnimation *m_slideAni;

    dcc::ContentWidget *m_content;
};

#endif // FRAMEWIDGET_H
