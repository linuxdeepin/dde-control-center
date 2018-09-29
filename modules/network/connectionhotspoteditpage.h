#ifndef CONNECTIONHOTSPOTEDITPAGE_H
#define CONNECTIONHOTSPOTEDITPAGE_H

#include "connectioneditpagenew.h"

namespace dcc {
namespace network {

class ConnectionHotspotEditPage : public ConnectionEditPageNew
{
    Q_OBJECT

public:
    explicit ConnectionHotspotEditPage(const QString &devPath, const QString &connUuid = QString(), QWidget *parent = 0);
    virtual ~ConnectionHotspotEditPage();

    void initSettingsWidget() Q_DECL_OVERRIDE;

private:
    QString m_devPath;
};

} /* network */ 
} /* dcc */ 

#endif /* CONNECTIONHOTSPOTEDITPAGE_H */
