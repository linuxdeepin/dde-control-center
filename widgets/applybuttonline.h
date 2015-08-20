#ifndef APPLYBUTTONLINE_H
#define APPLYBUTTONLINE_H

#include <QWidget>
#include <QHBoxLayout>
#include "libdui/dtextbutton.h"
#include "libdui/dheaderline.h"

DUI_USE_NAMESPACE

class ApplyButtonLine : public DHeaderLine
{
    Q_OBJECT
public:
    explicit ApplyButtonLine(QWidget *parent = 0);

signals:
    void cancel();
    void apply();
};
#endif // APPLYBUTTONLINE_H
