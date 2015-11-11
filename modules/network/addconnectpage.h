#ifndef ADDCONNECTPAGE_H
#define ADDCONNECTPAGE_H

#include "listwidgetcontainer.h"

class AddConnectPage : public ListWidgetContainer
{
    Q_OBJECT
public:
    explicit AddConnectPage(QWidget *parent = 0);

private:
    void init();
};

#endif // ADDCONNECTPAGE_H
