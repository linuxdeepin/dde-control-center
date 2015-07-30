#include <QLabel>
#include <QDebug>

#include "defaultapps.h"

DefaultApps::DefaultApps()
{
    m_label = new QLabel("DefaultApps");
}

DefaultApps::~DefaultApps()
{
    qDebug() << "~DefaultApps()";

    m_label->deleteLater();
}

QFrame* DefaultApps::getContent()
{
    return m_label;
}
