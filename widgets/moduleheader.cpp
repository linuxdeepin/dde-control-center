#include <QLabel>
#include <QDebug>

#include <libdui/dthememanager.h>
#include <libdui/dtextbutton.h>

#include "moduleheader.h"

ModuleHeader::ModuleHeader(QString title, QWidget * parent) :
    DBaseLine(parent)
{
    D_THEME_INIT_WIDGET(ModuleHeader);

    this->setFixedHeight(48);

    m_title = new QLabel(title, this);
    m_reset = new DTextButton("Reset", this);

    this->setLeftContent(m_title);
    this->setRightContent(m_reset);
}

