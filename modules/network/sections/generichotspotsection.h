#ifndef GENERICHOTSPOTSECTION_H
#define GENERICHOTSPOTSECTION_H

#include "abstractsection.h"
#include "lineeditwidget.h"

#include <networkmanagerqt/connectionsettings.h>

namespace dcc {
namespace widgets {

class GenericHotspotSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit GenericHotspotSection(NetworkManager::ConnectionSettings::Ptr connSettings, QFrame *parent = 0);
    virtual ~GenericHotspotSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initUI();

private:
    LineEditWidget *m_connIdItem;

    NetworkManager::ConnectionSettings::Ptr m_connSettings;
};

} /* widgets */
} /* dcc */

#endif /* GENERICHOTSPOTSECTION_H */
