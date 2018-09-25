#ifndef VPNSTRONGSWANSETTINGS_H
#define VPNSTRONGSWANSETTINGS_H

#include "../abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

namespace dcc {
namespace widgets {

class VpnStrongSwanSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit VpnStrongSwanSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent = 0);
    virtual ~VpnStrongSwanSettings();

protected:
    void initSections() Q_DECL_OVERRIDE;
};

} /* widgets */
} /* dcc */

#endif /* VPNSTRONGSWANSETTINGS_H */
