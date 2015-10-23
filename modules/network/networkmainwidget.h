#ifndef NETWORKMAINWIDGET_H
#define NETWORKMAINWIDGET_H

#include "scrollframe.h"

class QVBoxLayout;
class NetworkMainWidget : public ScrollFrame
{
    Q_OBJECT
public:
    explicit NetworkMainWidget(QWidget *parent = 0);

private:
    void initUI();
};

#endif // NETWORKMAINWIDGET_H
