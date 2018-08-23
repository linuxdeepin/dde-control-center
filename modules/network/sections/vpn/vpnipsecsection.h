#ifndef VPNIPSECSECTION_H
#define VPNIPSECSECTION_H

#include "../abstractsection.h"
#include "contentwidget.h"
#include "switchwidget.h"
#include "lineeditwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace widgets {

class VpnIpsecSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnIpsecSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnIpsecSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initConnection();
    void onIpsecCheckedChanged(const bool enabled);

private:
    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    SwitchWidget *m_ipsecEnable;
    LineEditWidget *m_groupName;
    LineEditWidget *m_gatewayId;
    LineEditWidget *m_psk;
    LineEditWidget *m_ike;
    LineEditWidget *m_esp;
};

} /* widgets */
} /* dcc */

#endif /* VPNIPSECSECTION_H */
