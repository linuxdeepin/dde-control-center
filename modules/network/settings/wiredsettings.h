#ifndef WIREDSETTINGS_H
#define WIREDSETTINGS_H

#include "abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

namespace dcc {
namespace widgets {

class WiredSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit WiredSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent = 0);
    virtual ~WiredSettings();

protected:
    void initSections() Q_DECL_OVERRIDE;
};

} /* widgets */
} /* dcc */

#endif /* WIREDSETTINGS_H */
