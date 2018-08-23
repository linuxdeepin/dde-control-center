#ifndef WIRELESSSETTINGS_H
#define WIRELESSSETTINGS_H

#include "abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

namespace dcc {
namespace widgets {

class WirelessSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit WirelessSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent = 0);
    virtual ~WirelessSettings();

protected:
    void initSections() Q_DECL_OVERRIDE;
};

} /* widgets */
} /* dcc */

#endif /* WIRELESSSETTINGS_H */
