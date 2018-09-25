#ifndef VPNOPENVPNSETTINGS_H
#define VPNOPENVPNSETTINGS_H

#include "../abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

namespace dcc {
namespace widgets {

class VpnOpenVPNSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit VpnOpenVPNSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent = 0);
    virtual ~VpnOpenVPNSettings();

protected:
    void initSections() Q_DECL_OVERRIDE;
};

} /* widgets */
} /* dcc */

#endif /* VPNOPENVPNSETTINGS_H */
