#ifndef GENERALREMOVEBUTTON_H
#define GENERALREMOVEBUTTON_H

#include <QWidget>
#include "libdui/dimagebutton.h"
#include "libdui/dthememanager.h"

DUI_USE_NAMESPACE

class GeneralRemoveButton : public DImageButton
{
    Q_OBJECT
public:
    explicit GeneralRemoveButton(QWidget *parent = 0);
};

#endif // GENERALREMOVEBUTTON_H
