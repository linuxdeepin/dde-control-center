#include <libdui/dthememanager.h>

#include "dtextaction.h"

DUI_USE_NAMESPACE

DTextAction::DTextAction(QWidget *parent) :
    DLinkButton("", parent)
{
    D_THEME_INIT_WIDGET(DTextAction);
}
