#include <libdui/dthememanager.h>

#include "imagenamebutton.h"

ImageNameButton::ImageNameButton(const QString &iconName, QWidget *parent) :
    DImageButton(parent)
{
    QString icon_path = "widgets/themes/";
    icon_path += DThemeManager::instance()->theme().append("/images/").append(iconName);
    setNormalPic(icon_path+"_normal.png");
    setPressPic(icon_path+"_press.png");
    setHoverPic(icon_path+"_hover.png");
    setCheckedPic(icon_path+"_checked.png");
}

