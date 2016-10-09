
#include "frame.h"
#include "framewidget.h"

#include <QResizeEvent>
#include <QTimer>
#include <QVBoxLayout>

FrameWidget::FrameWidget(Frame *parent)
    : QWidget(parent),

      m_slideAni(new QPropertyAnimation(this, "pos")),
      m_content(nullptr)
{
    m_slideAni->setEasingCurve(QEasingCurve::InOutCubic);
    m_slideAni->setDuration(200);

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->setSpacing(0);
    centeralLayout->setMargin(0);
    setLayout(centeralLayout);

    parent->installEventFilter(this);
    setFixedSize(parent->size());
}

QWidget *FrameWidget::setContent(QWidget * const c)
{
    QWidget *lastContent = m_content;
    m_content = c;
    layout()->addWidget(c);

    return lastContent;
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

void FrameWidget::destory()
{
    m_slideAni->setStartValue(QPoint());
    m_slideAni->setEndValue(QPoint(width(), 0));
    m_slideAni->start();

    QTimer::singleShot(m_slideAni->duration(), this, &FrameWidget::destorySelf);
}

bool FrameWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parent() && event->type() == QEvent::Resize)
        setFixedSize(static_cast<QResizeEvent *>(event)->size());

    return false;
}

void FrameWidget::destorySelf()
{
    if (m_content)
        m_content->setParent(nullptr);

    deleteLater();
}
