#ifndef CONNECTIONWIRELESSEDITPAGE_H
#define CONNECTIONWIRELESSEDITPAGE_H

#include "connectioneditpagenew.h"

#include <networkmanagerqt/accesspoint.h>

namespace dcc {
namespace network {

class ConnectionWirelessEditPage : public ConnectionEditPageNew
{
    Q_OBJECT

public:
    explicit ConnectionWirelessEditPage(const QString &devPath, const QString &connUuid = QString(), QWidget *parent = 0);
    virtual ~ConnectionWirelessEditPage();

    // This method must be called after initialization
    void initSettingsWidgetFromAp(const QString &apPath);

private:
    void initApSecretType(NetworkManager::AccessPoint::Ptr nmAp);
};

} /* network */ 
} /* dcc */ 

#endif /* CONNECTIONWIRELESSEDITPAGE_H */
