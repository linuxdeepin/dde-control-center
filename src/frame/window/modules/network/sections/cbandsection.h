#ifndef CBANDSECTION_H
#define CBANDSECTION_H

#include "window/modules/network/sections/abstractsection.h"
#include <networkmanagerqt/connectionsettings.h>
#include <networkmanagerqt/wirelesssetting.h>
#include <networkmanagerqt/wirelessdevice.h>


namespace dcc {
namespace widgets {
class ComboxWidget;
}
}
using dcc::widgets::ComboxWidget;
namespace DCC_NAMESPACE {
namespace network {


class CbandSection : public AbstractSection
{
    Q_OBJECT
public:
    CbandSection(NetworkManager::WirelessSetting::Ptr wiredSetting, QFrame *parent);
    virtual ~CbandSection();
    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initUi();
    NetworkManager::WirelessSetting::Ptr m_settings;
    QHash<QString, NetworkManager::WirelessSetting::FrequencyBand> m_bandList;
private:
    ComboxWidget *m_bandChooser{nullptr};
};

}
}

#endif // CBANDSECTION_H
