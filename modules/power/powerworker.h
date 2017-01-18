#ifndef POWERWORKER_H
#define POWERWORKER_H

#include <com_deepin_daemon_power.h>
#include <QObject>

using PowerInter = com::deepin::daemon::Power;

namespace dcc{
namespace power {
class PowerModel;
class PowerWorker : public QObject
{
    Q_OBJECT

public:
    explicit PowerWorker(PowerModel *model, QObject *parent = 0);

    void active();
    void deactive();

public slots:
    void setScreenBlackLock(const bool lock);
    void setSleepLock(const bool lock);
    void setSleepOnLidClosed(const bool sleep);
    void setSleepDelay(const int delay);
    void setScreenBlackDelay(const int delay);
    void setSleepDelayToModel(const int delay);
    void setScreenBlackDelayToModel(const int delay);

private:
    int  converToDelayModel(int value);
    int  converToDelayDBus(int value);

private:
    PowerModel *m_powerModel;
    PowerInter *m_powerInter;
};

}
}


#endif // POWERWORKER_H
