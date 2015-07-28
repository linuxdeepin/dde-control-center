#include "dtipsframe.h"
#include "constants.h"

#include <QHBoxLayout>
#include <QPropertyAnimation>

DTipsFrame::DTipsFrame()
    : QFrame(0)
{
    setFixedSize(100, 30);
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);

    m_label = new QLabel(this);
    m_label->setAlignment(Qt::AlignCenter);
#ifdef QT_DEBUG // test text label
    m_label->setText("TestTips");
    m_label->setStyleSheet("background-color:red;");

    setStyleSheet("background-color:black;");
#endif

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_label);
    layout->setContentsMargins(8, 5, 8, 5);

    setLayout(layout);
}

void DTipsFrame::move(int x, int y)
{
    if (m_isFirstMove)
    {
        QFrame::move(x, y);
        m_isFirstMove = false;
    }
    else
    {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
        animation->setStartValue(geometry());
        animation->setEndValue(QRect(x, y, width(), height()));
        animation->setDuration(DCC::TipsAnimationDuration);
        animation->setEasingCurve(QEasingCurve::OutCubic);
        animation->start();
    }
}

void DTipsFrame::followTheSender()
{
    QWidget *widget = qobject_cast<QWidget *>(sender());

    if (!widget)
        return;

    const int wHeight = widget->height();

    QPoint pos;
    do
    {
        pos += widget->pos();
    } while ((widget = widget->parentWidget()) != 0);

    int x = pos.x() - width();
    int y = pos.y() + (wHeight - height()) / 2;

    move(x, y);
}

