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

