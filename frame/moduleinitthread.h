#ifndef MODULEINITTHREAD_H
#define MODULEINITTHREAD_H

#include "moduleinterface.h"

#include <QThread>

class ModuleInitThread : public QThread
{
    Q_OBJECT

public:
    explicit ModuleInitThread(dcc::ModuleInterface *inter, QObject *parent = 0);

signals:
    void moduleInitFinished(dcc::ModuleInterface *inter) const;

protected:
    void run();

private:
    dcc::ModuleInterface *m_module;
};

#endif // MODULEINITTHREAD_H
