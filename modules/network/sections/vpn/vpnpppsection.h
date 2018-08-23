#ifndef VPNPPPSECTION_H
#define VPNPPPSECTION_H

#include "../abstractsection.h"
#include "contentwidget.h"
#include "switchwidget.h"
#include "comboboxwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace widgets {

class VpnPPPSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnPPPSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnPPPSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

    void setSupportOptions(const QStringList &supportOptions);

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initConnection();
    void onMppeEnableChanged(const bool checked);
    void onMppeMethodChanged(const QString &method);

private:
    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    SwitchWidget *m_mppeEnable;
    ComboBoxWidget *m_mppeChooser;
    SwitchWidget *m_mppeStateful;

    QString m_currentMppeMethod;
    QStringList m_supportOptions;
    QList<SwitchWidget *> m_optionsWidgets;
};

} /* widgets */
} /* dcc */

#endif /* VPNPPPSECTION_H */
