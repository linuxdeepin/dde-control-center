#ifndef ETHERNETSECTION_H
#define ETHERNETSECTION_H

#include "abstractsection.h"
#include "comboboxwidget.h"
#include "lineeditwidget.h"
#include "switchwidget.h"
#include "spinboxwidget.h"

#include <networkmanagerqt/wiredsetting.h>
#include <networkmanagerqt/wireddevice.h>

namespace dcc {
namespace widgets {

class EthernetSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit EthernetSection(NetworkManager::WiredSetting::Ptr wiredSetting, QFrame *parent = 0);
    virtual ~EthernetSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initConnection();

    void onCostomMtuChanged(const bool enable);

private:
    ComboBoxWidget *m_deviceMac;
    LineEditWidget *m_clonedMac;
    SwitchWidget *m_customMtuSwitch;
    SpinBoxWidget *m_customMtu;

    NetworkManager::WiredSetting::Ptr m_wiredSetting;

    QRegExp m_macAddrRegExp;
    QMap<QString, QString> m_macStrMap;
};

} /* widgets */
} /* dcc */

#endif /* ETHERNETSECTION_H */
