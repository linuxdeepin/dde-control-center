#ifndef VPNOPENCONNECTSETTINGS_H
#define VPNOPENCONNECTSETTINGS_H

#include "../abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

namespace dcc {
namespace widgets {

class VpnOpenConnectSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit VpnOpenConnectSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent = 0);
    virtual ~VpnOpenConnectSettings();

protected:
    void initSections() Q_DECL_OVERRIDE;
};

} /* widgets */
} /* dcc */

#endif /* VPNOPENCONNECTSETTINGS_H */
