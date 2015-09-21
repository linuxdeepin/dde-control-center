#include "dtipsframe.h"
#include "constants.h"

#include <QHBoxLayout>
#include <QPropertyAnimation>

DTipsFrame::DTipsFrame()
    : QWidget(0)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    setAttribute(Qt::WA_TranslucentBackground);

    setFixedHeight(40);

    m_label = new QLabel(this);
    m_label->setAlignment(Qt::AlignCenter);
#ifdef QT_DEBUG // test text label
    m_label->setText("TestTips");
#endif
    updateStyle();

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
    if (m_isFirstMove) {
        QWidget::move(x, y);
        m_isFirstMove = false;
    } else {
        QWidget::move(x, geometry().y());

        m_moveAni->stop();
        m_moveAni->setStartValue(geometry());
        m_moveAni->setEndValue(QRect(x, y, width(), height()));
        m_moveAni->start();
    }
}

DTipsFrame::ArrowDirection DTipsFrame::arrowDirection() const
{
    return m_arrowDirection;
}

void DTipsFrame::updateStyle()
{
    if(m_arrowDirection == ArrowLeft){
        setStyleSheet("color: white;\
                               border-width:6px 15px 6px 20px;\
                               padding-left: 8px;\
                               border-image:url(:/resources/images/control_center_tooltip_left.png) 6 15 6 20 stretch;");
    }else{
        setStyleSheet("color: white;\
                               border-width:6px 20px 6px 15px;\
                               border-image:url(:/resources/images/control_center_tooltip_right.png) 6 20 6 15 stretch;");
    }
}

void DTipsFrame::followTheSender()
{
    QWidget *widget = qobject_cast<QWidget *>(sender());

    if (!widget) {
        return;
    }

    const int wHeight = widget->height();
    const int wWidth = arrowDirection() == ArrowLeft
            ? widget->window()->geometry().right() + 8
            : widget->window()->geometry().left() - width() - 8;

    QPoint pos;
    do {
        pos += widget->pos();
    } while ((widget = widget->parentWidget()) != 0);

    int x = wWidth;
    int y = pos.y() + (wHeight - height()) / 2;

    y += m_extraOffsetY;

    move(x, y);
}

void DTipsFrame::setTipsText(const QString &text)
{
    m_label->setText(text);

    QFontMetrics metric(m_label->font());

    setFixedWidth(metric.tightBoundingRect(text).width() + 40);
}

void DTipsFrame::show()
{
    QWidget::show();
}

void DTipsFrame::setArrowDirection(DTipsFrame::ArrowDirection arrowDirection)
{
    m_arrowDirection = arrowDirection;
    updateStyle();
}

