#include "generaladdbutton.h"

GeneralAddButton::GeneralAddButton(QWidget *parent) : DImageButton(parent)
{
    D_THEME_INIT_WIDGET(GeneralAddButton);
    setCheckable(false);
}

