#ifndef VPNL2TPSETTINGS_H
#define VPNL2TPSETTINGS_H

#include "../abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

namespace dcc {
namespace widgets {

class VpnL2tpSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit VpnL2tpSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent = 0);
    virtual ~VpnL2tpSettings();

protected:
    void initSections() Q_DECL_OVERRIDE;
};

} /* widgets */
} /* dcc */

#endif /* VPNL2TPSETTINGS_H */
