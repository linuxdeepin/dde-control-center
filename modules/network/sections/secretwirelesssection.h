#ifndef SECRETWIRELESSSECTION_H
#define SECRETWIRELESSSECTION_H

#include "secret8021xsection.h"
#include "comboboxwidget.h"
#include "passwdeditwidget.h"

#include <networkmanagerqt/wirelesssecuritysetting.h>

namespace dcc {
namespace widgets {

class SecretWirelessSection : public Secret8021xSection
{
    Q_OBJECT

public:
    SecretWirelessSection(NetworkManager::WirelessSecuritySetting::Ptr wsSeting,
            NetworkManager::Security8021xSetting::Ptr sSetting, QFrame *parent = 0);
    virtual ~SecretWirelessSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initUI();
    void initConnection();
    void onKeyMgmtChanged(NetworkManager::WirelessSecuritySetting::KeyMgmt);
    void saveUserInputPassword();

private:
    ComboBoxWidget *m_keyMgmtChooser;
    PasswdEditWidget *m_passwdEdit;
    Secret8021xEnableWatcher *m_enableWatcher;

    NetworkManager::WirelessSecuritySetting::KeyMgmt m_currentKeyMgmt;

    NetworkManager::WirelessSecuritySetting::Ptr m_wsSetting;
    NetworkManager::Security8021xSetting::Ptr m_s8Setting;

    QMap<NetworkManager::WirelessSecuritySetting::KeyMgmt, QString> m_userInputPasswordMap;
};

} /* widgets */
} /* dcc */

Q_DECLARE_METATYPE(NetworkManager::WirelessSecuritySetting::KeyMgmt);

#endif /* SECRETWIRELESSSECTION_H */
