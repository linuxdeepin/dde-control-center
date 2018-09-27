#ifndef VPNSECOPENVPNSECTION_H
#define VPNSECOPENVPNSECTION_H

#include "../abstractsection.h"
#include "contentwidget.h"
#include "comboboxwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace widgets {

class VpnSecOpenVPNSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnSecOpenVPNSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnSecOpenVPNSection();

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

    ComboBoxWidget *m_cipherChooser;
    ComboBoxWidget *m_hmacChooser;

    QString m_currentCipher;
    QString m_currentHMAC;
};

} /* widgets */
} /* dcc */

#endif /* VPNSECOPENVPNSECTION_H */
