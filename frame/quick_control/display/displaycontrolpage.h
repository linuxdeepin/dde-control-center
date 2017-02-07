#ifndef DISPLAYCONTROLPAGE_H
#define DISPLAYCONTROLPAGE_H

#include "display/displaymodel.h"
#include "display/displayworker.h"

#include <QWidget>

class DisplayControlPage : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayControlPage(QWidget *parent = 0);

private:
    dcc::display::DisplayModel *m_displayModel;
    dcc::display::DisplayWorker *m_displayWorker;
};

#endif // DISPLAYCONTROLPAGE_H
