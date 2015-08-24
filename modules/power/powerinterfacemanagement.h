#ifndef POWERINTERFACEMANAGEMENT
#define POWERINTERFACEMANAGEMENT
#include <QObject>
#include "dbus/dbuspower.h"

class PowerInterfaceManagement:public QObject
{
    Q_OBJECT
public:
    PowerInterfaceManagement(QObject *parent=0);
    ~PowerInterfaceManagement();

    com::deepin::daemon::DBusPower* mn_powerInterface;
signals:
    void PowerButtonActionChanged();
    void LidClosedActionChanged();
public slots:
    void initialConnection();
    qint32 getPowerButtonAction();
    qint32 getLidCloseAction();
    void setPowerButtonAction(QString actionButton);
    void setLidCloseAction(QString actionButton);


};
#endif // POWERINTERFACEMANAGEMENT

