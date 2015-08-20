#ifndef SWITCHLINE_H
#define SWITCHLINE_H

#include <QWidget>
#include "libdui/dseparatorhorizontal.h"
#include "libdui/dswitchbutton.h"
#include "libdui/dheaderline.h"

DUI_USE_NAMESPACE

class SwitchLine : public DHeaderLine
{
    Q_OBJECT
public:
    explicit SwitchLine(QWidget *parent = 0);
    bool check() const;
    void setCheck(bool s);

signals:
    void checkChanged(bool s);

private:
    void setContent(QWidget *content);

private:
    DSwitchButton *m_button = NULL;
};

#endif // SWITCHLINE_H
