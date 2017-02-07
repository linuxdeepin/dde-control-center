#ifndef DISPLAYCONTROLPAGE_H
#define DISPLAYCONTROLPAGE_H

#include "display/displaymodel.h"
#include "display/displayworker.h"

#include <QWidget>

class DisplayControlPage : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayControlPage(dcc::display::DisplayModel *model, QWidget *parent = 0);
};

#endif // DISPLAYCONTROLPAGE_H
