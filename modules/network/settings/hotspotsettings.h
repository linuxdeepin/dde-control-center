#ifndef HOTSPOTSETTINGS_H
#define HOTSPOTSETTINGS_H

#include "abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

namespace dcc {
namespace widgets {

class HotspotSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit HotspotSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent = 0);
    virtual ~HotspotSettings();

protected:
    void initSections() Q_DECL_OVERRIDE;
};

} /* widgets */
} /* dcc */

#endif /* HOTSPOTSETTINGS_H */
