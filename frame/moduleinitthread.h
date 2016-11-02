#ifndef MODULEINITTHREAD_H
#define MODULEINITTHREAD_H

#include "moduleinterface.h"

#include <QThread>

class ModuleInitThread : public QThread
{
    Q_OBJECT

public:
    explicit ModuleInitThread(ModuleInterface *inter, QObject *parent = 0);

signals:
    void moduleInitFinished(ModuleInterface *inter) const;

protected:
    void run();

private:
    ModuleInterface *m_module;
};

#endif // MODULEINITTHREAD_H
