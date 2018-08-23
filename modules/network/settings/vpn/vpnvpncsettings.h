#ifndef VPNVPNCSETTINGS_H
#define VPNVPNCSETTINGS_H

#include "../abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

namespace dcc {
namespace widgets {

class VpnVPNCSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit VpnVPNCSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent = 0);
    virtual ~VpnVPNCSettings();

protected:
    void initSections() Q_DECL_OVERRIDE;
};

} /* widgets */
} /* dcc */

#endif /* VPNVPNCSETTINGS_H */
