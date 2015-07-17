#include <QLabel>
#include <QDebug>

#include "bluetooth.h"

Bluetooth::Bluetooth()
{
    m_label = new QLabel("Bluetooth");
}

Bluetooth::~Bluetooth()
{
    qDebug() << "~Bluetooth()";

    m_label->deleteLater();
}

QFrame* Bluetooth::getContent()
{
    return m_label;
}
