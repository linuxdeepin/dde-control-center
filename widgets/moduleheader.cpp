#include <QLabel>
#include <QDebug>

#include <libdui/dthememanager.h>
#include <libdui/dtextbutton.h>

#include "moduleheader.h"

ModuleHeader::ModuleHeader(QString title, bool addResetButton, QWidget * parent) :
    DBaseLine(parent)
{
    D_THEME_INIT_WIDGET(ModuleHeader);

    this->setFixedHeight(48);

    m_title = new QLabel(title, this);
    this->setLeftContent(m_title);

    if (addResetButton) {
        m_reset = new DTextButton("Reset", this);
        this->setRightContent(m_reset);

        connect(m_reset, &DTextButton::clicked, this, &ModuleHeader::resetButtonClicked);
    }
}

