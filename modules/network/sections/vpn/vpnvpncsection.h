#ifndef VPNVPNCSECTION_H
#define VPNVPNCSECTION_H

#include "../abstractsection.h"
#include "contentwidget.h"
#include "lineeditwidget.h"
#include "comboboxwidget.h"
#include "passwdeditwidget.h"
#include "switchwidget.h"
#include "filechoosewidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace widgets {

class VpnVPNCSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnVPNCSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnVPNCSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initConnection();
    void onPasswordFlagsChanged(NetworkManager::Setting::SecretFlagType type);
    void onGroupPasswordFlagsChanged(NetworkManager::Setting::SecretFlagType type);

private:
    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NetworkManager::Setting::SecretFlagType m_currentPasswordType;
    NetworkManager::Setting::SecretFlagType m_currentGroupPasswordType;
    NMStringMap m_dataMap;
    NMStringMap m_secretMap;

    LineEditWidget *m_gateway;
    LineEditWidget *m_userName;
    ComboBoxWidget *m_passwordFlagsChooser;
    PasswdEditWidget *m_password;

    LineEditWidget *m_groupName;
    ComboBoxWidget *m_groupPasswordFlagsChooser;
    PasswdEditWidget *m_groupPassword;
    SwitchWidget *m_userHybrid;
    FileChooseWidget *m_caFile;
};

} /* widgets */
} /* dcc */

Q_DECLARE_METATYPE(NetworkManager::Setting::SecretFlagType);

#endif /* VPNVPNCSECTION_H */
