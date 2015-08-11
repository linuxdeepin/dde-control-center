#include <QDebug>
#include <QLabel>

#include "grub.h"
#include "grubwidget.h"

Grub::Grub():
    m_grubWidget(new GrubWidget())
{

}

Grub::~Grub()
{
    qDebug() << "~Grub()";
    delete m_grubWidget ;
}

QFrame *Grub::getContent()
{
    return m_grubWidget;
}

