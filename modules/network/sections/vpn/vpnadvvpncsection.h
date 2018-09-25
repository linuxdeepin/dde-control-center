#ifndef VPNADVVPNCSECTION_H
#define VPNADVVPNCSECTION_H

#include "../abstractsection.h"
#include "contentwidget.h"
#include "lineeditwidget.h"
#include "comboboxwidget.h"
#include "spinboxwidget.h"
#include "switchwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace widgets {

class VpnAdvVPNCSection : public AbstractSection
{
    Q_OBJECT

public:
    VpnAdvVPNCSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnAdvVPNCSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initConnection();

private:
    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    LineEditWidget *m_domain;
    ComboBoxWidget *m_vendorChooser;
    LineEditWidget *m_version;
    ComboBoxWidget *m_encryptionChooser;
    ComboBoxWidget *m_natTravModeChooser;
    ComboBoxWidget *m_ikeDHGroupChooser;
    ComboBoxWidget *m_forwordSecrecyChooser;
    SpinBoxWidget *m_localPort;
    SwitchWidget *m_disableDPD;

    QString m_currentVendor;
    QString m_currentEncryption;
    QString m_currentNatTravMod;
    QString m_currentIkeDHGroup;
    QString m_currentForwordSecrecy;
};

} /* widgets */
} /* dcc */

#endif /* VPNADVVPNCSECTION_H */
