#include "umountbutton.h"

UmountButton::UmountButton(QWidget *parent) : QLabel(parent)
{
    setFixedSize(16,16);

    QPixmap buttonMap("://Resource/images/unmount_button.png");
    setPixmap(buttonMap.scaled(size()));
}

void UmountButton::mousePressEvent(QMouseEvent *)
{
    emit mousePressed();
}

void UmountButton::enterEvent(QEvent *)
{
    emit mouseHovered();
}

void UmountButton::leaveEvent(QEvent *)
{
    emit mouseExited();
}
