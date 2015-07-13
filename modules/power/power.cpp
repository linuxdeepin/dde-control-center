#include <QLabel>
#include <QDebug>

#include "power.h"

Power::Power()
{
    m_label = new QLabel("Power");
}

Power::~Power()
{
    qDebug() << "~Power()";

    m_label->deleteLater();
}

QFrame* Power::getContent()
{
    return m_label;
}
