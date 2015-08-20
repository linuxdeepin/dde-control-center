#include "monitor.h"

Monitor::Monitor(QWidget *parent) :
    QLabel(parent)
{
    setStyleSheet("QLabel {"
                  "border: 1px solid white;"
                  "background-color: grey;"
                  "font-size: 18px;"
                  "color: white;"
                  "}");

    setAlignment(Qt::AlignCenter);
}


void Monitor::setName(QString name)
{
    setText(name);
}

QSize Monitor::resolution()
{
    return m_resolution;
}

void Monitor::setResolution(int width, int height)
{
    m_resolution = QSize(width, height);
}
