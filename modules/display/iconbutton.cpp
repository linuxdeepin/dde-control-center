#include "iconbutton.h"

IconButton::IconButton(QWidget *parent) : QPushButton(parent)
{

}

void IconButton::setNormalIcon(const QIcon &icon)
{
    m_normal = icon;

    setIcon(icon);
}

void IconButton::setHoverIcon(const QIcon &icon)
{
    m_hover = icon;
}

void IconButton::setPressIcon(const QIcon &icon)
{
    m_press = icon;
}

void IconButton::enterEvent(QEvent *e)
{
    QPushButton::enterEvent(e);

    setIcon(m_hover);
}

void IconButton::mousePressEvent(QMouseEvent *e)
{
    QPushButton::mousePressEvent(e);

    setIcon(m_press);
}

void IconButton::leaveEvent(QEvent *e)
{
    QPushButton::leaveEvent(e);

    setIcon(m_normal);
}
