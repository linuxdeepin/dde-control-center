#include "multideletebutton.h"

MultiDeleteButton::MultiDeleteButton(QWidget *parent) : DImageButton(parent)
{
    D_THEME_INIT_WIDGET(MultiDeleteButton);
    setCheckable(false);
}

