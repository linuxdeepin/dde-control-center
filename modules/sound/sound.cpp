#include <QLabel>
#include <QDebug>

#include "sound.h"

Sound::~Sound()
{
    qDebug() << "~Sound";
}

QFrame* Sound::getContent()
{
    QLabel* label = new QLabel("sound"); 
    return label;
}
