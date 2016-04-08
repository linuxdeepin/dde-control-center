#include "doublecornerwidget.h"
#include <QDebug>

DoubleCornerWidget::DoubleCornerWidget(QWidget *parent)
    : QWidget(parent)
{
    this->setFixedSize(400, 400);
    this->update();

    setStyleSheet("background: transparent;");
}

DoubleCornerWidget::~DoubleCornerWidget()
{

}

void DoubleCornerWidget::setDirection(Direction direct) {
    m_direct = direct;
}

void DoubleCornerWidget::setRadiusMargins(int rad, int margins) {
    m_radius = rad;
    m_margins = margins;
}

void DoubleCornerWidget::setContentLayout(QLayout* l) {
    setLayout(l);
}
void DoubleCornerWidget::paintEvent(QPaintEvent *e) {
    Q_UNUSED(e);
    QRect widgetRect = QRect(this->rect().x() + m_margins, this->rect().y() + m_margins,
                             this->rect().width() - m_margins*2, this->rect().height() - m_margins*2);
    const int &radius = m_radius;
    QPoint leftTopStartPos = QPoint(widgetRect.x() + radius*2, widgetRect.y());
//    QPoint leftTopEndPos = QPoint(widgetRect.x(), widgetRect.y() + radius);

//    QPoint leftBottomEndPos = QPoint(widgetRect.x() + radius, widgetRect.y() + widgetRect.height());
    QPoint leftBottomStartPos = QPoint(widgetRect.x() + radius, widgetRect.y() + widgetRect.height() - radius*2);

//    QPoint rightTopEndPos = QPoint(widgetRect.x() + widgetRect.width() - radius, widgetRect.y());
    QPoint rightTopStartPos = QPoint(widgetRect.x() + widgetRect.width(), widgetRect.y() + radius);

//    QPoint rightBottomEndPos = QPoint(widgetRect.x() + widgetRect.width(), widgetRect.y() + widgetRect.height() - radius);
    QPoint rightBottomStartPos = QPoint(widgetRect.x() + widgetRect.width() - radius*2, widgetRect.y() + widgetRect.height());

    QPen pen(Qt::transparent);
    pen.setWidth(2);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath path;
    if (m_direct == Top) {
        path.moveTo(leftTopStartPos.x() - radius, leftTopStartPos.y());
        path.arcTo(leftTopStartPos.x() - radius*2, leftTopStartPos.y(), radius*2, radius*2, 90, 90);
        path.lineTo(widgetRect.x(), widgetRect.y() + widgetRect.height());
        path.lineTo(widgetRect.x() + widgetRect.width(),  widgetRect.y() + widgetRect.height());
        path.lineTo(rightTopStartPos.x() , rightTopStartPos.y());
        path.arcTo(rightTopStartPos.x() - radius*2, rightTopStartPos.y() - radius, radius*2, radius*2, 0, 90);
        path.lineTo(leftTopStartPos.x() - radius, leftTopStartPos.y());
    } else if (m_direct == Bottom){
        path.moveTo(widgetRect.x(), widgetRect.y());
        path.lineTo(leftBottomStartPos.x() - radius, leftBottomStartPos.y());
        path.arcTo(leftBottomStartPos.x() - radius, leftBottomStartPos.y(),  radius*2, radius*2, 180, 90);
        path.lineTo(rightBottomStartPos.x(), rightBottomStartPos.y());
        path.arcTo(rightBottomStartPos.x(), rightBottomStartPos.y() - radius*2, radius*2, radius*2, 270, 90);
        path.lineTo(rightTopStartPos.x() , rightTopStartPos.y()- radius);
        path.lineTo(widgetRect.x(), widgetRect.y());
    } else if (m_direct == All) {
        path.moveTo(leftTopStartPos.x() - radius, leftTopStartPos.y());
        path.arcTo(leftTopStartPos.x() - radius*2, leftTopStartPos.y(), radius*2, radius*2, 90, 90);
        path.lineTo(leftBottomStartPos.x() - radius, leftBottomStartPos.y());
        path.arcTo(leftBottomStartPos.x() - radius, leftBottomStartPos.y(), radius*2, radius*2, 180, 90);
        path.lineTo(rightBottomStartPos.x(), rightBottomStartPos.y());
        path.arcTo(rightBottomStartPos.x(), rightBottomStartPos.y() - radius*2, radius*2, radius*2, 270, 90);
        path.lineTo(rightTopStartPos.x(), rightTopStartPos.y() - radius);
        path.arcTo(rightTopStartPos.x() - radius*2, rightTopStartPos.y() - radius, radius*2, radius*2, 0, 90);
        path.lineTo(leftTopStartPos.x() - radius, leftTopStartPos.y());
    } else {
        path.moveTo(widgetRect.x(), widgetRect.y());
        path.lineTo(widgetRect.x(), widgetRect.y() + widgetRect.height());
        path.lineTo(widgetRect.x() + widgetRect.width(), widgetRect.y() + widgetRect.height());
        path.lineTo(widgetRect.x() + widgetRect.width(), widgetRect.y());
        path.lineTo(widgetRect.x(), widgetRect.y());
    }
    painter.setClipPath(path);
    painter.setBrush(DCC::BgDarkColor);
    painter.setPen(pen);
    painter.drawPath(path);

    QPen strokePen;
    strokePen.setColor(Qt::transparent);
    strokePen.setWidth(2);
    painter.strokePath(path, strokePen);
}
