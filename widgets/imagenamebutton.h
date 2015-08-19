#ifndef IMAGENAMEBUTTON_H
#define IMAGENAMEBUTTON_H

#include <libdui/dimagebutton.h>
#include <libdui/libdui_global.h>

DUI_USE_NAMESPACE

class ImageNameButton : public DImageButton
{
public:
    explicit ImageNameButton(const QString &iconName, QWidget *parent = 0);
};

#endif // IMAGENAMEBUTTON_H
