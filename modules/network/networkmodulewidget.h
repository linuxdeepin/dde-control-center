#ifndef NETWORKMODULEWIDGET_H
#define NETWORKMODULEWIDGET_H

#include "modulewidget.h"

namespace dcc
{
class NextPageWidget;
class SettingsGroup;

namespace network {

class NetworkDevice;
class NetworkModel;
class NetworkModuleWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit NetworkModuleWidget();

    void setModel(NetworkModel *model);

signals:
    void requestShowDetailPage() const;

private slots:
    void onDeviceListChanged(const QList<NetworkDevice *> &devices);

private:
    dcc::SettingsGroup *m_devicesLayout;
    dcc::NextPageWidget *m_pppBtn;
    dcc::NextPageWidget *m_vpnBtn;
    dcc::NextPageWidget *m_proxyBtn;
    dcc::NextPageWidget *m_detailBtn;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKMODULEWIDGET_H
