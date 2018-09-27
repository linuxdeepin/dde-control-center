#ifndef VPNOPENCONNECTSECTION_H
#define VPNOPENCONNECTSECTION_H

#include "../abstractsection.h"
#include "contentwidget.h"
#include "lineeditwidget.h"
#include "filechoosewidget.h"
#include "switchwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace widgets {

class VpnOpenConnectSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnOpenConnectSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnOpenConnectSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();

private:
    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    LineEditWidget *m_gateway;
    FileChooseWidget *m_caCert;
    LineEditWidget *m_proxy;
    SwitchWidget *m_enableCSDTrojan;
    LineEditWidget *m_csdScript;
    FileChooseWidget *m_userCert;
    FileChooseWidget *m_userKey;
    SwitchWidget *m_useFSID;
};

} /* widgets */
} /* dcc */

#endif /* VPNOPENCONNECTSECTION_H */
