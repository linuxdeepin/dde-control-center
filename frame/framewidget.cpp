
#include "frame.h"
#include "framewidget.h"
#include "contentwidget.h"

#include <QResizeEvent>
#include <QTimer>
#include <QVBoxLayout>

using namespace dcc;

FrameWidget::FrameWidget(Frame *parent)
    : TranslucentFrame(parent),

      m_opacityEffect(new QGraphicsOpacityEffect(this)),
      m_slidePosAni(new QPropertyAnimation(this, "pos")),
      m_content(nullptr)
{
    m_opacityEffect->setOpacity(1.0);
    m_slidePosAni->setEasingCurve(QEasingCurve::InOutCubic);
    m_slidePosAni->setDuration(300);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->setSpacing(0);
    centralLayout->setMargin(0);
    setLayout(centralLayout);

    parent->installEventFilter(this);
    setFixedHeight(parent->size().height());
    setFixedWidth(FRAME_WIDTH);
    setGraphicsEffect(m_opacityEffect);

    // change widget opacity
    connect(m_slidePosAni, &QPropertyAnimation::valueChanged, this, [this](const QVariant &value) {
        m_opacityEffect->setOpacity(1.0 - std::abs(double(value.toPoint().x())) / FRAME_WIDTH);
    });
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

    QTimer::singleShot(m_slidePosAni->duration(), this, &QWidget::hide);
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

void FrameWidget::destory()
{
    m_slidePosAni->setStartValue(QPoint());
    m_slidePosAni->setEndValue(QPoint(width(), 0));
    m_slidePosAni->start();

    QTimer::singleShot(m_slidePosAni->duration(), this, &FrameWidget::destorySelf);
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

void FrameWidget::destorySelf()
{
    if (m_content)
    {
        m_content->setParent(nullptr);
        emit contentDetached(m_content);
    }

    deleteLater();
}
