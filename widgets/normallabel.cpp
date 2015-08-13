#include <libdui/dthememanager.h>

#include "normallabel.h"

DUI_USE_NAMESPACE

NormalLabel::NormalLabel(QWidget *parent)
    : QLabel(parent)
{
    D_THEME_INIT_WIDGET(NormalLabel);
}

NormalLabel::NormalLabel(QString title, QWidget *parent)
    : QLabel(title, parent)
{
    D_THEME_INIT_WIDGET(NormalLabel);
}
