#include "dtipsframe.h"
#include "constants.h"

#include <QHBoxLayout>
#include <QPropertyAnimation>

DTipsFrame::DTipsFrame()
    : QFrame(0)
{
    setFixedHeight(40);
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    setAttribute(Qt::WA_TranslucentBackground);

    m_label = new QLabel(this);
    m_label->setAlignment(Qt::AlignCenter);
#ifdef QT_DEBUG // test text label
    m_label->setText("TestTips");
#endif
    m_label->setStyleSheet(QString("border-width:6px 20px 6px 15px; border-image:url(%1) 6 20 6 15 stretch; color:#fff;").arg(DCC::IconPath + "control_center_tooltip.png"));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_label);
    layout->setMargin(0);

    m_moveAni = new QPropertyAnimation(this, "geometry");
    m_moveAni->setEasingCurve(DCC::TipsMoveCurve);
    m_moveAni->setDuration(DCC::TipsMoveAnimationDuration);

    setLayout(layout);
}

DTipsFrame::~DTipsFrame()
{
    m_moveAni->deleteLater();
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
        QFrame::move(x, geometry().y());

        m_moveAni->stop();
        m_moveAni->setStartValue(geometry());
        m_moveAni->setEndValue(QRect(x, y, width(), height()));
        m_moveAni->start();
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

    x += m_extraOffsetX;
    y += m_extraOffsetY;

    move(x, y);
}

void DTipsFrame::setTipsText(const QString &text)
{
    m_label->setText(text);

    QFontMetrics metric(m_label->font());

    setFixedWidth(metric.tightBoundingRect(text).width() + 50);
}

