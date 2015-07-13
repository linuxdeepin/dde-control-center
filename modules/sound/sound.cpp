#include <QLabel>
#include <QDebug>

#include "sound.h"

Sound::Sound()
{
    m_label = new QLabel("Sound");
}

Sound::~Sound()
{
    qDebug() << "~Sound";
    m_label->deleteLater();
}

QFrame* Sound::getContent()
{
    return m_label;
}
