#include <QLabel>
#include <QDebug>

#include "network.h"

Network::Network()
{
    m_label = new QLabel("Network");
}

Network::~Network()
{
    qDebug() << "~Network()";

    m_label->deleteLater();
}

QFrame* Network::getContent()
{
    return m_label;
}
