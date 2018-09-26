#ifndef VPNADVOPENVPNSECTION_H
#define VPNADVOPENVPNSECTION_H

#include "../abstractsection.h"
#include "contentwidget.h"
#include "switchwidget.h"
#include "spinboxwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace widgets {

class VpnAdvOpenVPNSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnAdvOpenVPNSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnAdvOpenVPNSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initConnection();

private:
    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    SwitchWidget *m_portSwitch;
    SwitchWidget *m_renegIntervalSwitch;
    SwitchWidget *m_compLZOSwitch;
    SwitchWidget *m_tcpProtoSwitch;
    SwitchWidget *m_useTapSwitch;
    SwitchWidget *m_tunnelMTUSwitch;
    SwitchWidget *m_udpFragSizeSwitch;
    SwitchWidget *m_restrictMSSSwitch;
    SwitchWidget *m_randomRemoteSwitch;

    SpinBoxWidget *m_port;
    SpinBoxWidget *m_renegInterval;
    SpinBoxWidget *m_tunnelMTU;
    SpinBoxWidget *m_udpFragSize;
};

} /* widgets */
} /* dcc */

#endif /* VPNADVOPENVPNSECTION_H */
