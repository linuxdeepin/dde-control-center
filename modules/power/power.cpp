#include <QLabel>
#include <QDebug>

#include "power.h"

Power::~Power()
{
    qDebug() << "~Power()";
}

QFrame* Power::getContent()
{
    QLabel* label = new QLabel("power"); 
    return label;
}
