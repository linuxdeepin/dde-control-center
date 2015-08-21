#ifndef MULTIDELETEBUTTON_H
#define MULTIDELETEBUTTON_H

#include <QWidget>
#include "libdui/dimagebutton.h"
#include "libdui/dthememanager.h"

DUI_USE_NAMESPACE

class MultiDeleteButton : public DImageButton
{
    Q_OBJECT
public:
    explicit MultiDeleteButton(QWidget *parent = 0);
};

#endif // MULTIDELETEBUTTON_H
