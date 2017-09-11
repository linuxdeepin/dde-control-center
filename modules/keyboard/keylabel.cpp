#include "keylabel.h"

#include <QApplication>
#include <QPainter>

KeyLabel::KeyLabel(QWidget *parent)
    : QLabel(parent)
    , m_isEnter(false)
{
    setAlignment(Qt::AlignCenter);
}

KeyLabel::KeyLabel(const QString &text, QWidget *parent)
    : KeyLabel(parent)
{
    setText(text);

    QFont font = qApp->font();
    QFontMetrics fm(font);

    setMinimumWidth(fm.width(text) + 8);
}

void KeyLabel::setEnter(const bool enter)
{
    m_isEnter = enter;

    update();
}

void KeyLabel::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

    QPainter painter(this);

    painter.setOpacity(m_isEnter ? 1.0 : 0.85);

    painter.setBrush(QColor(255, 255, 255, 0.2*255));
    painter.setPen(Qt::NoPen);

    QFont font = qApp->font();
    QFontMetrics fm(font);

    int h = (height() - fm.height()) / 2;

    painter.drawRoundRect(rect().adjusted(0, h, 0, -h));
}
