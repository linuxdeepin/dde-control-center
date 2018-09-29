#ifndef DSLPPPOESETTINGS_H
#define DSLPPPOESETTINGS_H

#include "abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

namespace dcc {
namespace widgets {

class DslPppoeSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit DslPppoeSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent = 0);
    virtual ~DslPppoeSettings();

protected:
    void initSections() Q_DECL_OVERRIDE;
};

} /* widgets */
} /* dcc */

#endif /* DSLPPPOESETTINGS_H */
