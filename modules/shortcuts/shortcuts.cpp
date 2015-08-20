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
    m_mainWidget->setParent(0);
    m_mainWidget->deleteLater();
}

QFrame *Shortcuts::getContent()
{
    return m_mainWidget;
}

