#ifndef ADDCONNECTPAGE_H
#define ADDCONNECTPAGE_H

#include <libdui/dboxwidget.h>
#include <libdui/libdui_global.h>

DUI_USE_NAMESPACE

class AddConnectPage : public DVBoxWidget
{
    Q_OBJECT
public:
    explicit AddConnectPage(QWidget *parent = 0);

private:
    void init();
};

#endif // ADDCONNECTPAGE_H
