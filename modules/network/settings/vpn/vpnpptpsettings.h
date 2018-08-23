#ifndef VPNPPTPSETTINGS_H
#define VPNPPTPSETTINGS_H

#include "../abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

namespace dcc {
namespace widgets {

class VpnPPTPSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit VpnPPTPSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent = 0);
    virtual ~VpnPPTPSettings();

protected:
    void initSections() Q_DECL_OVERRIDE;
};

} /* widgets */
} /* dcc */

#endif /* VPNPPTPSETTINGS_H */
