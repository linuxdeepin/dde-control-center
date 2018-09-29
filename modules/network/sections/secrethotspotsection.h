#ifndef SECRETHOTSPOTSECTION_H
#define SECRETHOTSPOTSECTION_H

#include "abstractsection.h"
#include "comboboxwidget.h"
#include "passwdeditwidget.h"

#include <networkmanagerqt/wirelesssecuritysetting.h>

namespace dcc {
namespace widgets {

class SecretHotspotSection : public AbstractSection
{
    Q_OBJECT

public:
    SecretHotspotSection(NetworkManager::WirelessSecuritySetting::Ptr wsSeting, QFrame *parent = 0);
    virtual ~SecretHotspotSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initConnection();
    void onKeyMgmtChanged(NetworkManager::WirelessSecuritySetting::KeyMgmt);
    void saveUserInputPassword();

private:
    ComboBoxWidget *m_keyMgmtChooser;
    PasswdEditWidget *m_passwdEdit;

    NetworkManager::WirelessSecuritySetting::KeyMgmt m_currentKeyMgmt;

    NetworkManager::WirelessSecuritySetting::Ptr m_wsSetting;

    QMap<NetworkManager::WirelessSecuritySetting::KeyMgmt, QString> m_userInputPasswordMap;
};

} /* widgets */
} /* dcc */

Q_DECLARE_METATYPE(NetworkManager::WirelessSecuritySetting::KeyMgmt);

#endif /* SECRETHOTSPOTSECTION_H */
