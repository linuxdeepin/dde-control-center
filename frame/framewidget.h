#ifndef FRAMEWIDGET_H
#define FRAMEWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

#include <DWindowManagerHelper>

#include "translucentframe.h"

DWIDGET_USE_NAMESPACE

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
    void destroy();

    int animationDuration() const;

signals:
    void contentDetached(dcc::ContentWidget * const w) const;

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void destroySelf();
    void onCompositeChanged();

private:
#ifndef DISABLE_OPACITY_ANIMATION
    QGraphicsOpacityEffect *m_opacityEffect;
#endif
    QPropertyAnimation *m_slidePosAni;

    dcc::ContentWidget *m_content;

    DWindowManagerHelper *m_wmHelper;
};

#endif // FRAMEWIDGET_H
