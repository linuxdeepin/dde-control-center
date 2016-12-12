#ifndef NETWORKMODULEWIDGET_H
#define NETWORKMODULEWIDGET_H

#include "modulewidget.h"

namespace dcc{
namespace widgets {
class NextPageWidget;
class SettingsGroup;
}

namespace network {

class NetworkDevice;
class NetworkModel;
class NetworkModuleWidget : public dcc::ModuleWidget
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
    dcc::widgets::SettingsGroup *m_devicesLayout;
    dcc::widgets::NextPageWidget *m_pppBtn;
    dcc::widgets::NextPageWidget *m_vpnBtn;
    dcc::widgets::NextPageWidget *m_proxyBtn;
    dcc::widgets::NextPageWidget *m_detailBtn;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKMODULEWIDGET_H
