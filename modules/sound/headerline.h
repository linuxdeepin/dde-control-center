#ifndef HEADERLINE_H
#define HEADERLINE_H

#include <libdui/dbaseline.h>

DUI_USE_NAMESPACE

class HeaderLine : public DBaseLine
{
    Q_OBJECT
public:
    explicit HeaderLine(QString title, QWidget *parent = 0);
};

#endif // HEADERLINE_H
