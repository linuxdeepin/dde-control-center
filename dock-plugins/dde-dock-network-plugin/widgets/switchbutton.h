#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include <QRadioButton>

class SwitchButton : public QRadioButton
{
    Q_OBJECT
public:
    SwitchButton(QWidget *parent = 0);
};

#endif // SWITCHBUTTON_H
