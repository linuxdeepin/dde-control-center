#include <QDebug>
#include <QFrame>

#include "display.h"

Display::Display():
    m_frame(NULL)
{

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

