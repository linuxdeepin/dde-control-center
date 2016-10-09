#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include "modulewidget.h"
#include "frameproxyinterface.h"

#include <QPushButton>

class Accounts : public ModuleWidget
{
    Q_OBJECT

public:
    explicit Accounts(FrameProxyInterface *frame);

private:
    QPushButton *m_nextPage;
};

#endif // ACCOUNTS_H
