#ifndef GENERICSECTION_H
#define GENERICSECTION_H

#include "abstractsection.h"
#include "lineeditwidget.h"
#include "switchwidget.h"

#include <networkmanagerqt/connectionsettings.h>

namespace dcc {
namespace widgets {

class GenericSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit GenericSection(NetworkManager::ConnectionSettings::Ptr connSettings, QFrame *parent = 0);
    virtual ~GenericSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initUI();

private:
    LineEditWidget *m_connIdItem;
    SwitchWidget *m_autoConnItem;

    NetworkManager::ConnectionSettings::Ptr m_connSettings;
};

} /* widgets */
} /* dcc */

#endif /* GENERICSECTION_H */
