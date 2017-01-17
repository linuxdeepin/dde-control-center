#ifndef NETWORKMODULEWIDGET_H
#define NETWORKMODULEWIDGET_H

#include "modulewidget.h"
#include "nextpagewidget.h"

namespace dcc{
namespace widgets {
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
    void requestShowVpnPage() const;
    void requestShowPppPage() const;
    void requestShowProxyPage() const;
    void requestShowInfomation() const;
    void requestShowDeviceDetail(NetworkDevice *device) const;
    void requestDeviceEnable(const QString &devPath, const bool enable) const;

private slots:
    void onDeviceListChanged(const QList<NetworkDevice *> &devices);
    void onNextPageClicked();

private:
    void createDeviceGroup(NetworkDevice *dev, const int number, const bool multiple);

private:
    QVBoxLayout *m_devicesLayout;
    dcc::widgets::NextPageWidget *m_pppBtn;
    dcc::widgets::NextPageWidget *m_vpnBtn;
    dcc::widgets::NextPageWidget *m_proxyBtn;
    dcc::widgets::NextPageWidget *m_detailBtn;

    QMap<dcc::widgets::NextPageWidget *, NetworkDevice *> m_devices;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKMODULEWIDGET_H
