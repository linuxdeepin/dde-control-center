#include <QFile>

#include "switchbutton.h"

SwitchButton::SwitchButton(QWidget *parent) :
    QRadioButton(parent)
{
    QFile file(":/qss/SwitchButton.qss");
    if (file.open(QIODevice::ReadOnly)) {
        setStyleSheet(QString(file.readAll()));
    }
}

