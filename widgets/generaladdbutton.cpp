#include "generaladdbutton.h"

GeneralAddButton::GeneralAddButton(QWidget *parent) : DImageButton(parent)
{
    D_THEME_INIT_WIDGET(GeneralAddButton);
    setCheckable(false);
}

void GeneralAddButton::enterEvent(QEvent *)
{
    emit mouseEnter();
}

void GeneralAddButton::leaveEvent(QEvent *)
{
    emit mouseLeave();
}

