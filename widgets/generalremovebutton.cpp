#include "generalremovebutton.h"

GeneralRemoveButton::GeneralRemoveButton(QWidget *parent) : DImageButton(parent)
{
    D_THEME_INIT_WIDGET(GeneralRemoveButton);

    setCheckable(true);
    setFixedSize(24, 23);   //image's size
}

void GeneralRemoveButton::enterEvent(QEvent *)
{
    emit mouseEnter();
}

void GeneralRemoveButton::leaveEvent(QEvent *)
{
    emit mouseLeave();
}

