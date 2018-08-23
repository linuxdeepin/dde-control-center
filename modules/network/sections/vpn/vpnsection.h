#ifndef VPNSECTION_H
#define VPNSECTION_H

#include "../abstractsection.h"
#include "contentwidget.h"
#include "lineeditwidget.h"
#include "comboboxwidget.h"
#include "passwdeditwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace widgets {

class VpnSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initConnection();
    void onPasswordFlagsChanged(NetworkManager::Setting::SecretFlagType type);

private:
    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NetworkManager::Setting::SecretFlagType m_currentPasswordType;
    NMStringMap m_dataMap;
    NMStringMap m_secretMap;

    LineEditWidget *m_gateway;
    LineEditWidget *m_userName;
    ComboBoxWidget *m_passwordFlagsChooser;
    PasswdEditWidget *m_password;
    LineEditWidget *m_domain;
};

} /* widgets */
} /* dcc */

Q_DECLARE_METATYPE(NetworkManager::Setting::SecretFlagType);

#endif /* VPNSECTION_H */
