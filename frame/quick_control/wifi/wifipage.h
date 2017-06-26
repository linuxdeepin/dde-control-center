#ifndef WIFIPAGE_H
#define WIFIPAGE_H

#include "wifipage.h"
#include "network/networkmodel.h"

#include <QWidget>

class WifiPage : public QWidget
{
    Q_OBJECT

public:
    explicit WifiPage(dcc::network::NetworkModel *model, QWidget *parent = 0);

signals:
    void mouseLeaveView() const;
    void requestConnectHidden(const QString &module, const QString &page) const;
    void requestDeviceApList(const QString &devPath) const;
    void requestDeactivateConnection(const QString &uuid) const;
    void requestActivateAccessPoint(const QString &devPath, const QString &apPath, const QString &uuid) const;

private:
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void onItemClicked(const QModelIndex &index);

private:
    dcc::network::NetworkModel *m_networkModel;
};

#endif // WIFIPAGE_H
