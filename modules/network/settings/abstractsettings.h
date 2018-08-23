#ifndef ABSTRACTSETTINGS_H
#define ABSTRACTSETTINGS_H

#include "contentwidget.h"
#include "../sections/abstractsection.h"

#include <networkmanagerqt/connectionsettings.h>

#include <QWidget>

namespace dcc {
namespace widgets {

class AbstractSettings : public QWidget
{
    Q_OBJECT

public:
    explicit AbstractSettings(NetworkManager::ConnectionSettings::Ptr connSettings, QWidget *parent = 0);
    virtual ~AbstractSettings();

    bool allInputValid();
    void saveSettings();

Q_SIGNALS:
    void requestNextPage(ContentWidget * const page);

protected:
    virtual void initSections() = 0;

protected:
    NetworkManager::ConnectionSettings::Ptr m_connSettings;

    QVBoxLayout *m_sectionsLayout;
    QList<dcc::widgets::AbstractSection *> m_settingSections;
};

} /* widgets */
} /* dcc */

#endif /* ABSTRACTSETTINGS_H */
