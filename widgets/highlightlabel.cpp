#include <libdui/dthememanager.h>

#include "highlightlabel.h"

DUI_USE_NAMESPACE

HighlightLabel::HighlightLabel(QWidget *parent)
    : QLabel(parent)
{
    D_THEME_INIT_WIDGET(HighlightLabel);
}

HighlightLabel::HighlightLabel(QString text, QWidget *parent)
    : QLabel(text, parent)
{
    D_THEME_INIT_WIDGET(HighlightLabel);
}
