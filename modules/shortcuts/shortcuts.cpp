#include <QDebug>
#include <QLabel>

#include "mainwidget.h"
#include "shortcuts.h"

Shortcuts::Shortcuts():
    m_mainWidget(new MainWidget())
{

}

Shortcuts::~Shortcuts()
{
    qDebug() << "~Shortcuts()";
    delete m_mainWidget ;
}

QFrame *Shortcuts::getContent()
{
    return m_mainWidget;
}

