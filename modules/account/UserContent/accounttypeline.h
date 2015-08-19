#ifndef ACCOUNTTYPELINE_H
#define ACCOUNTTYPELINE_H

#include <QWidget>
#include "libdui/dheaderline.h"
#include "libdui/dsegmentedcontrol.h"
#include "libdui/dseparatorhorizontal.h"

DUI_USE_NAMESPACE

class AccountTypeLine : public DHeaderLine
{
    Q_OBJECT
public:
    explicit AccountTypeLine(QWidget *parent = 0);
    void setType(int type);

signals:
    void typeChanged(int type);

private:
    DSegmentedControl *m_control = NULL;
};

#endif // ACCOUNTTYPELINE_H
