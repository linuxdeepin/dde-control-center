#ifndef POWERWORKER_H
#define POWERWORKER_H

#include "powermodel.h"

#include <com_deepin_daemon_power.h>

#include <QObject>

using PowerInter = com::deepin::daemon::Power;

class PowerWorker : public QObject
{
    Q_OBJECT

public:
    explicit PowerWorker(PowerModel *model, QObject *parent = 0);

    void active();
    void deactive();

public slots:
    void setScreenBlackLock(const bool lock);

private:
    PowerModel *m_powerModel;
    PowerInter *m_powerInter;
};

#endif // POWERWORKER_H
