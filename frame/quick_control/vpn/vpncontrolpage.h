#ifndef VPNCONTROLPAGE_H
#define VPNCONTROLPAGE_H

#include "vpnlistmodel.h"

#include <QWidget>

class VpnControlPage : public QWidget
{
    Q_OBJECT

public:
    explicit VpnControlPage(dcc::network::NetworkModel *model, QWidget *parent = 0);

signals:
    void requestActivateConnection(const QString &devPath, const QString &uuid) const;

private slots:
    void onItemClicked(const QModelIndex &index) const;

private:
    VpnListModel *m_listModel;
};

#endif // VPNCONTROLPAGE_H
