#include "switchline.h"

SwitchLine::SwitchLine(QWidget *parent) : DHeaderLine(parent)
{
    m_button = new DSwitchButton(this);
    connect(m_button, &DSwitchButton::checkedChanged, this, &SwitchLine::checkChanged);
    setContent(m_button);
    DSeparatorHorizontal *sh = new DSeparatorHorizontal(this);
    sh->setFixedWidth(width());
    sh->move(0, height() - 2);
}

void SwitchLine::setCheck(bool s)
{
    m_button->setChecked(s);
}

void SwitchLine::setContent(QWidget *content)
{
    DHeaderLine::setContent(content);
}

