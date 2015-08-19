#include "accounttypeline.h"

AccountTypeLine::AccountTypeLine(QWidget *parent) : DHeaderLine(parent)
{
    m_control = new DSegmentedControl(this);
    m_control->addSegmented("Normal User");
    m_control->addSegmented("Administor");
    connect(m_control, &DSegmentedControl::currentChanged, this, &AccountTypeLine::typeChanged);
    setContent(m_control);
    DSeparatorHorizontal * separator = new DSeparatorHorizontal(this);
    separator->setFixedWidth(width());
    separator->move(0, height() - 2);
}

void AccountTypeLine::setType(int type)
{
    m_control->setCurrentIndex(type);
}

