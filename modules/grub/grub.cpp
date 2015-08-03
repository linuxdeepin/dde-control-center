#include "grub.h"
#include <QDebug>
#include "grubwidget.h"
#include <QLabel>

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

