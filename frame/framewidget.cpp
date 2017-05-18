
#include "frame.h"
#include "framewidget.h"
#include "contentwidget.h"

#include <QResizeEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <DWindowManagerHelper>

using namespace dcc;

FrameWidget::FrameWidget(Frame *parent)
    : TranslucentFrame(parent),

#ifndef DISABLE_OPACITY_ANIMATION
      m_opacityEffect(new QGraphicsOpacityEffect(this)),
#endif
      m_slidePosAni(new QPropertyAnimation(this, "pos")),
      m_content(nullptr),
      m_wmHelper(DWindowManagerHelper::instance())
{
#ifndef DISABLE_OPACITY_ANIMATION
    m_opacityEffect->setOpacity(1.0);
#endif
    m_slidePosAni->setEasingCurve(QEasingCurve::InOutCubic);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->setSpacing(0);
    centralLayout->setMargin(0);
    setLayout(centralLayout);

    parent->installEventFilter(this);
    setFixedHeight(parent->size().height());
    setFixedWidth(FRAME_WIDTH);

#ifndef DISABLE_OPACITY_ANIMATION
    setGraphicsEffect(m_opacityEffect);

    // change widget opacity
    connect(m_slidePosAni, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
        qreal opacity = qBound(qreal(0.0), 1.0 - qAbs(qreal(value.toPoint().x())) / FRAME_WIDTH, qreal(1.0));

        m_opacityEffect->setEnabled(!qFuzzyCompare(opacity, 1.0));
        m_opacityEffect->setOpacity(opacity);
    });
#endif
    connect(m_wmHelper, &DWindowManagerHelper::hasCompositeChanged, this, &FrameWidget::onCompositeChanged);

    QTimer::singleShot(1, this, &FrameWidget::onCompositeChanged);
}

ContentWidget *FrameWidget::setContent(ContentWidget * const c)
{
    ContentWidget *lastContent = m_content;
    m_content = c;
    layout()->addWidget(c);

    return lastContent;
}

ContentWidget *FrameWidget::content()
{
    return m_content;
}

void FrameWidget::show()
{
    QWidget::show();

    m_slidePosAni->setStartValue(QPoint(width(), 0));
    m_slidePosAni->setEndValue(QPoint());
    m_slidePosAni->start();
}

void FrameWidget::hide()
{
    m_slidePosAni->setStartValue(QPoint());
    m_slidePosAni->setEndValue(QPoint(-width(), 0));
    m_slidePosAni->start();

    // ignore mouse events
    setAttribute(Qt::WA_TransparentForMouseEvents);
    QTimer::singleShot(m_slidePosAni->duration(), [=] { setAttribute(Qt::WA_TransparentForMouseEvents, false); QWidget::hide(); });
}

void FrameWidget::showBack()
{
    QWidget::show();

    m_slidePosAni->setStartValue(QPoint(-width(), 0));
    m_slidePosAni->setEndValue(QPoint());
    m_slidePosAni->start();
}

void FrameWidget::hideBack()
{
    m_slidePosAni->setStartValue(QPoint());
    m_slidePosAni->setEndValue(QPoint(width(), 0));
    m_slidePosAni->start();

    QTimer::singleShot(m_slidePosAni->duration(), this, &QWidget::hide);
}

void FrameWidget::destroy()
{
    m_slidePosAni->setStartValue(QPoint());
    m_slidePosAni->setEndValue(QPoint(width(), 0));
    m_slidePosAni->start();

    QTimer::singleShot(m_slidePosAni->duration(), this, &FrameWidget::destroySelf);
}

int FrameWidget::animationDuration() const
{
    return m_slidePosAni->duration();
}

bool FrameWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parent() && event->type() == QEvent::Resize)
        setFixedHeight(static_cast<QResizeEvent *>(event)->size().height());

    return false;
}

void FrameWidget::destroySelf()
{
    if (m_content)
    {
        m_content->setParent(nullptr);
        emit contentDetached(m_content);
    }

    deleteLater();
}

void FrameWidget::onCompositeChanged()
{
    m_slidePosAni->setDuration(m_wmHelper->hasComposite() ? 300 : 150);
}
