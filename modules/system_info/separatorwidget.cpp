#include "separatorwidget.h"

#include <libdui/dthememanager.h>

DUI_USE_NAMESPACE

SeparatorWidget::SeparatorWidget(QWidget *parent)
    : QFrame(parent)
{
    D_THEME_INIT_WIDGET(SeparatorWidget);
}

HSeparatorWidget::HSeparatorWidget(QWidget *parent)
    : SeparatorWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(1);
}

VSeparatorWidget::VSeparatorWidget(QWidget *parent)
    : SeparatorWidget(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setFixedWidth(1);
}
