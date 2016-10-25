#include "display.h"

#include <QPushButton>

Display::Display()
    : ModuleWidget()
{

    for (int i(0); i != 20; ++i)
    {
        QPushButton *b = new QPushButton;
        b->setText("test " + QString::number(i));
        m_centeralLayout->addWidget(b);
    }

    setTitle(tr("Display"));
}
