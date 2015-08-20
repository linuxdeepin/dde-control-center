#ifndef CONFIRMBUTTONLINE_H
#define CONFIRMBUTTONLINE_H

#include <QWidget>
#include <QHBoxLayout>
#include "libdui/dtextbutton.h"
#include "libdui/dheaderline.h"

DUI_USE_NAMESPACE

class ConfirmButtonLine : public DHeaderLine
{
    Q_OBJECT
public:
    explicit ConfirmButtonLine(QWidget *parent = 0);

signals:
    void cancel();
    void confirm();
};
#endif // CONFIRMBUTTONLINE_H
