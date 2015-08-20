#include <libdui/dthememanager.h>

#include "imagenamebutton.h"

ImageNameButton::ImageNameButton(const QString &iconName, QWidget *parent) :
    DImageButton(parent)
{
    QString icon_path = "widgets/themes/";
    icon_path += DThemeManager::instance()->theme().append("/images/").append(iconName);
    setNormalPic(QPixmap(icon_path+"_normal.png"));
    setPressPic(QPixmap(icon_path+"_press.png"));
    setHoverPic(QPixmap(icon_path+"_hover.png"));
}

