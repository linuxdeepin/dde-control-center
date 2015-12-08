#include "dtipsframe.h"
#include "constants.h"

#include <QHBoxLayout>
#include <QCoreApplication>
#include <QPropertyAnimation>
#include <QDebug>

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
        setStyleSheet("color: white; font-size: 12px;\
                               border-width:6px 15px 6px 20px;\
                               padding-left: 6px;\
                               border-image:url(:/resources/images/control_center_tooltip_left.png) 6 15 6 20 stretch;");
    }else{
        setStyleSheet("color: white; font-size: 12px;\
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
            ? widget->window()->geometry().right() - 12
            : widget->window()->geometry().left() - width() + 12;

    QPoint pos;
    do {
        pos += widget->pos();
    } while ((widget = widget->parentWidget()) != 0);

    int x = wWidth;
    int y = pos.y() + (wHeight - height()) / 2;

    move(x, y);
    show();
}

void DTipsFrame::showTipsTextAt(const QString &text, const QPoint &p)
{
    hide();

    m_label->setText(QCoreApplication::translate("ModuleName", text.toLatin1()));
    QFontMetrics metric(m_label->font());
    setFixedWidth(metric.tightBoundingRect(m_label->text()).width() + 40);

    int x = p.x();
    x += arrowDirection() == ArrowLeft ? rect().right() + 12
                                       : rect().left() - width() - 12;
    int y = p.y();
    y += 3;
    move(x, y);

    show();
}

void DTipsFrame::show()
{
    if (!isVisible())
        QWidget::show();
}

void DTipsFrame::setArrowDirection(DTipsFrame::ArrowDirection arrowDirection)
{
    m_arrowDirection = arrowDirection;
    updateStyle();
}

