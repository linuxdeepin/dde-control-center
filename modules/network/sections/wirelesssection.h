#ifndef WIRELESSSECTION_H
#define WIRELESSSECTION_H

#include "abstractsection.h"
#include "comboboxwidget.h"
#include "lineeditwidget.h"
#include "switchwidget.h"
#include "spinboxwidget.h"

#include <networkmanagerqt/wirelesssetting.h>
#include <networkmanagerqt/wirelessdevice.h>

namespace dcc {
namespace widgets {

class WirelessSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit WirelessSection(NetworkManager::WirelessSetting::Ptr wirelessSetting, QFrame *parent = 0);
    virtual ~WirelessSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initConnection();

    void onCostomMtuChanged(const bool enable);

private:
    LineEditWidget *m_apSsid;
    ComboBoxWidget *m_deviceMac;
    //LineEditWidget *m_clonedMac;
    SwitchWidget *m_customMtuSwitch;
    SpinBoxWidget *m_customMtu;

    NetworkManager::WirelessSetting::Ptr m_wirelessSetting;

    QRegExp m_macAddrRegExp;
    QMap<QString, QString> m_macStrMap;
};

} /* widgets */
} /* dcc */

#endif /* WIRELESSSECTION_H */
