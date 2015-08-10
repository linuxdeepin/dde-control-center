#include "shortcuts.h"
#include <QDebug>
#include "mainwidget.h"
#include <QLabel>

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

