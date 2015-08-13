#include <QDebug>
#include <QFrame>

#include "display.h"
#include "singlemonitorsettings.h"

Display::Display():
    m_frame(NULL)
{
    m_frame = new SingleMonitorSettings("");
    m_frame->setFixedWidth(300);
    m_frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

Display::~Display()
{
    qDebug() << "~Display()";
    delete m_frame ;
}

QFrame *Display::getContent()
{
    return m_frame;
}

