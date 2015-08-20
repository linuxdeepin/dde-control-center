#include <libdui/dthememanager.h>

#include "multiaddcheckbutton.h"

MultiAddCheckButton::MultiAddCheckButton(QWidget *parent) :
    DImageButton(parent),
    m_checked(false)
{
    updateIcon();
    connect(this, &MultiAddCheckButton::clicked, [&]{
       setChecked(!m_checked);
    });
}

bool MultiAddCheckButton::checked() const
{
    return m_checked;
}

void MultiAddCheckButton::setChecked(bool checked)
{
    if (m_checked == checked)
        return;

    m_checked = checked;
    updateIcon();
    emit checkedChanged(checked);
}

void MultiAddCheckButton::updateIcon()
{
    QString icon_path = "widgets/themes/";
    icon_path += DThemeManager::instance()->theme().append("/images/multiselect_");

    if(m_checked){
        setNormalPic(QPixmap(icon_path+"active_normal.png"));
        setPressPic(QPixmap(icon_path+"active_normal.png"));
        setHoverPic(QPixmap(icon_path+"active_normal.png"));
    }else{
        setNormalPic(QPixmap(icon_path+"inactive_normal.png"));
        setPressPic(QPixmap(icon_path+"inactive_normal.png"));
        setHoverPic(QPixmap(icon_path+"inactive_hover.png"));
    }
}

