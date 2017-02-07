#ifndef WIFIPAGE_H
#define WIFIPAGE_H

#include "network/networkmodel.h"

#include <QWidget>

class WifiPage : public QWidget
{
    Q_OBJECT

public:
    explicit WifiPage(dcc::network::NetworkModel *model, QWidget *parent = 0);

signals:
    void requestDeviceApList(const QString &devPath) const;

private:
    dcc::network::NetworkModel *m_networkModel;
};

#endif // WIFIPAGE_H
