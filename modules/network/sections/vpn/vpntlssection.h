#ifndef VPNTLSSECTION_H
#define VPNTLSSECTION_H

#include "../abstractsection.h"
#include "contentwidget.h"
#include "lineeditwidget.h"
#include "comboboxwidget.h"
#include "filechoosewidget.h"
#include "switchwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace widgets {

class VpnTLSSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnTLSSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnTLSSection();

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

    LineEditWidget *m_remote;
    ComboBoxWidget *m_remoteCertTypeChooser;
    FileChooseWidget *m_caCert;
    SwitchWidget *m_customKeyDirection;
    ComboBoxWidget *m_keyDirectionChooser;

    QString m_currentRemoteCertType;
    QString m_currentKeyDirection;
};

} /* widgets */
} /* dcc */

#endif /* VPNTLSSECTION_H */
