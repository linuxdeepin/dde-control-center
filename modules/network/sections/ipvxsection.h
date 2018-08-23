#ifndef IPVXSECTION_H
#define IPVXSECTION_H

#include "abstractsection.h"
#include "comboboxwidget.h"
#include "lineeditwidget.h"
#include "spinboxwidget.h"
#include "switchwidget.h"

#include <networkmanagerqt/ipv4setting.h>
#include <networkmanagerqt/ipv6setting.h>

namespace dcc {
namespace widgets {

class IpvxSection : public AbstractSection
{
    Q_OBJECT

public:
    enum Ipvx {Ipv4, Ipv6};

    explicit IpvxSection(NetworkManager::Ipv4Setting::Ptr ipv4Setting, QFrame *parent = 0);
    explicit IpvxSection(NetworkManager::Ipv6Setting::Ptr ipv6Setting, QFrame *parent = 0);
    virtual ~IpvxSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

    void saveIpv4Settings();
    void saveIpv6Settings();

    void setIpv4ConfigMethodEnable(NetworkManager::Ipv4Setting::ConfigMethod method, const bool enabled);
    void setIpv6ConfigMethodEnable(NetworkManager::Ipv6Setting::ConfigMethod method, const bool enabled);
    void setNeverDefaultEnable(const bool neverDefault);

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initForIpv4();
    void initForIpv6();
    void initConnection();

    void onIpv4MethodChanged(NetworkManager::Ipv4Setting::ConfigMethod method);
    void onIpv6MethodChanged(NetworkManager::Ipv6Setting::ConfigMethod method);

    bool ipv4InputIsValid();
    bool ipv6InputIsValid();
    bool isIpv4Address(const QString &ip);
    bool isIpv6Address(const QString &ip);

    QList<QHostAddress> dnsList();

private:
    ComboBoxWidget *m_methodChooser;
    LineEditWidget *m_ipAddress;
    LineEditWidget *m_netmaskIpv4;
    SpinBoxWidget *m_prefixIpv6;
    LineEditWidget *m_gateway;
    LineEditWidget *m_dnsPrimary;
    LineEditWidget *m_dnsSecond;
    SwitchWidget *m_neverDefault;

    QList<SettingsItem *> m_itemsList;

    Ipvx m_currentIpvx;
    NetworkManager::Setting::Ptr m_ipvxSetting;
};

} /* widgets */
} /* dcc */

Q_DECLARE_METATYPE(NetworkManager::Ipv4Setting::ConfigMethod);
Q_DECLARE_METATYPE(NetworkManager::Ipv6Setting::ConfigMethod);

#endif /* IPVXSECTION_H */
