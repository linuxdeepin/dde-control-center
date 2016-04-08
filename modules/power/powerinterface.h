#ifndef POWERINTERFACE_H
#define POWERINTERFACE_H

#include <QObject>
#include "dbus/dbuspower.h"

class PowerInterface: public QObject {
    Q_OBJECT
public:
    PowerInterface(QObject* parent=0);
    ~PowerInterface();

signals:
    void screenBlackRequirePassWd(bool require);
    void lidCloseRequirePassWd(bool require);
    void sleepRequiredPassWd(bool require);
    void batteryPercentageChanged(QList<BatteryItem> bats);

public slots:
    int getScreenBlackDelay();
    void setScreenBlackDelay(int time);

    int getSleepDelay();
    void setSleepDelay(int time);

    bool getScreenBlackNeedPassWd();
    void setScreenBlackNeedPassWd(bool need);
    bool getSleepNeedPassWd();
    void setSleepNeedPassWd(bool need);

    void setLidCloseNeedPassWd(bool need);
    bool getLidCloseNeedPassWd();

    void handleScreenBlackNeedPassWd();
    void handleSleepNeedPassWd();
    void handleLidCloseAction();
    void handleBatteryPercentageChanged();
    QList<BatteryItem> getBatteryInfos();

    DBusPower* getDBusPowerIterface();
private:
    DBusPower* m_dbusPower;

    bool lidCloseNeedpassword();
    void initConnection();
};
#endif // POWERINTERFACE_H
