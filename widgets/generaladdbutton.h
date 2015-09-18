#ifndef GENERALADDBUTTON_H
#define GENERALADDBUTTON_H

#include <QWidget>
#include <QEvent>
#include "libdui/dimagebutton.h"
#include "libdui/dthememanager.h"

DUI_USE_NAMESPACE

class GeneralAddButton : public DImageButton
{
    Q_OBJECT
public:
    explicit GeneralAddButton(QWidget *parent = 0);

signals:
    void mouseEnter();
    void mouseLeave();

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
};

#endif // GENERALADDBUTTON_H
