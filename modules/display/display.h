#ifndef DISPLAY_H
#define DISPLAY_H

#include "modulewidget.h"

class Display : public ModuleWidget
{
    Q_OBJECT

public:
    explicit Display(QWidget *parent = 0);
};

#endif // DISPLAY_H
