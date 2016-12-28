
#include "frame.h"
#include "framewidget.h"
#include "contentwidget.h"

#include <QResizeEvent>
#include <QTimer>
#include <QVBoxLayout>

using namespace dcc;

FrameWidget::FrameWidget(Frame *parent)
    : TranslucentFrame(parent),

      m_slideAni(new QPropertyAnimation(this, "pos")),
      m_content(nullptr)
{
    m_slideAni->setEasingCurve(QEasingCurve::InOutQuint);
    m_slideAni->setDuration(200);

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->setSpacing(0);
    centeralLayout->setMargin(0);
    setLayout(centeralLayout);

    parent->installEventFilter(this);
    setFixedHeight(parent->size().height());
    setFixedWidth(FRAME_WIDTH);
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

    m_slideAni->setStartValue(QPoint(width(), 0));
    m_slideAni->setEndValue(QPoint());
    m_slideAni->start();
}

void FrameWidget::hide()
{
    m_slideAni->setStartValue(QPoint());
    m_slideAni->setEndValue(QPoint(-width(), 0));
    m_slideAni->start();

    QTimer::singleShot(m_slideAni->duration(), this, &QWidget::hide);
}

void FrameWidget::showBack()
{
    QWidget::show();

    m_slideAni->setStartValue(QPoint(-width(), 0));
    m_slideAni->setEndValue(QPoint());
    m_slideAni->start();
}

void FrameWidget::hideBack()
{
    m_slideAni->setStartValue(QPoint());
    m_slideAni->setEndValue(QPoint(width(), 0));
    m_slideAni->start();

    QTimer::singleShot(m_slideAni->duration(), this, &QWidget::hide);
}

void FrameWidget::destory()
{
    m_slideAni->setStartValue(QPoint());
    m_slideAni->setEndValue(QPoint(width(), 0));
    m_slideAni->start();

    QTimer::singleShot(m_slideAni->duration(), this, &FrameWidget::destorySelf);
}

int FrameWidget::animationDuration() const
{
    return m_slideAni->duration();
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
