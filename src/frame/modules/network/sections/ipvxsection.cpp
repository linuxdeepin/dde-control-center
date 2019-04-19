/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ipvxsection.h"
#include "widgets/contentwidget.h"

#include <dspinbox.h>

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

IpvxSection::IpvxSection(NetworkManager::Ipv4Setting::Ptr ipv4Setting, QFrame *parent)
    : AbstractSection(tr("IPv4"), parent),
      m_methodChooser(new ComboBoxWidget(this)),
      m_ipAddress (new LineEditWidget(this)),
      m_netmaskIpv4 (new LineEditWidget(this)),
      m_prefixIpv6 (nullptr),
      m_gateway (new LineEditWidget(this)),
      m_dnsPrimary (new LineEditWidget(this)),
      m_dnsSecond (new LineEditWidget(this)),
      m_neverDefault(new SwitchWidget(this)),
      m_currentIpvx(Ipv4),
      m_ipvxSetting(ipv4Setting)
{
    initStrMaps();
    initUI();
    initConnection();

    onIpv4MethodChanged(Ipv4ConfigMethodStrMap.value(m_methodChooser->value()));
}

IpvxSection::IpvxSection(NetworkManager::Ipv6Setting::Ptr ipv6Setting, QFrame *parent)
    : AbstractSection(tr("IPv6"), parent),
      m_methodChooser(new ComboBoxWidget(this)),
      m_ipAddress (new LineEditWidget(this)),
      m_netmaskIpv4 (nullptr),
      m_prefixIpv6 (new SpinBoxWidget(this)),
      m_gateway (new LineEditWidget(this)),
      m_dnsPrimary (new LineEditWidget(this)),
      m_dnsSecond (new LineEditWidget(this)),
      m_neverDefault(new SwitchWidget(this)),
      m_currentIpvx(Ipv6),
      m_ipvxSetting(ipv6Setting)
{
    initStrMaps();
    initUI();
    initConnection();

    onIpv6MethodChanged(Ipv6ConfigMethodStrMap.value(m_methodChooser->value()));
}

IpvxSection::~IpvxSection()
{

}

bool IpvxSection::allInputValid()
{
    bool valid = true;

    switch (m_currentIpvx) {
        case Ipv4:
            valid = ipv4InputIsValid();
            break;
        case Ipv6:
            valid = ipv6InputIsValid();
            break;
        default:
            break;
    }

    return valid;
}

void IpvxSection::saveSettings()
{
    bool initialized = true;

    switch (m_currentIpvx) {
        case Ipv4:
            initialized = saveIpv4Settings();
            break;
        case Ipv6:
            initialized = saveIpv6Settings();
            break;
        default:
            break;
    }

    m_ipvxSetting->setInitialized(initialized);
}

bool IpvxSection::saveIpv4Settings()
{
    NetworkManager::Ipv4Setting::Ptr ipv4Setting = m_ipvxSetting.staticCast<NetworkManager::Ipv4Setting>();

    NetworkManager::Ipv4Setting::ConfigMethod method = Ipv4ConfigMethodStrMap.value(m_methodChooser->value());
    ipv4Setting->setMethod(method);

    if (method == NetworkManager::Ipv4Setting::Manual) {
        NetworkManager::IpAddress ipAddress;
        ipAddress.setIp(QHostAddress(m_ipAddress->text()));
        ipAddress.setNetmask(QHostAddress(m_netmaskIpv4->text()));
        ipAddress.setGateway(QHostAddress(m_gateway->text()));
        ipv4Setting->setAddresses(QList<NetworkManager::IpAddress>() << ipAddress);
    }

    const QList<QHostAddress> &mDnsList = dnsList();

    ipv4Setting->setDns(mDnsList);

    if (method == NetworkManager::Ipv4Setting::Automatic) {
        ipv4Setting->setAddresses(QList<NetworkManager::IpAddress>());
        ipv4Setting->setIgnoreAutoDns(!mDnsList.isEmpty());
    }

    if (m_neverDefault->isVisible()) {
        ipv4Setting->setNeverDefault(m_neverDefault->checked());
    }

    return true;
}

bool IpvxSection::saveIpv6Settings()
{
    NetworkManager::Ipv6Setting::Ptr ipv6Setting = m_ipvxSetting.staticCast<NetworkManager::Ipv6Setting>();

    NetworkManager::Ipv6Setting::ConfigMethod method = Ipv6ConfigMethodStrMap.value(m_methodChooser->value());
    ipv6Setting->setMethod(Ipv6ConfigMethodStrMap.value(m_methodChooser->value()));

    if (method == NetworkManager::Ipv6Setting::Ignored) {
        ipv6Setting->setAddresses(QList<NetworkManager::IpAddress>());
        return true;
    }

    if (method == NetworkManager::Ipv6Setting::Manual) {
        NetworkManager::IpAddress ipAddress;
        ipAddress.setIp(QHostAddress(m_ipAddress->text()));
        ipAddress.setPrefixLength(m_prefixIpv6->spinBox()->value());
        ipAddress.setGateway(QHostAddress(m_gateway->text()));
        ipv6Setting->setAddresses(QList<NetworkManager::IpAddress>() << ipAddress);
    }

    const QList<QHostAddress> &mDnsList = dnsList();

    ipv6Setting->setDns(mDnsList);

    if (method == NetworkManager::Ipv6Setting::Automatic) {
        ipv6Setting->setAddresses(QList<NetworkManager::IpAddress>());
        ipv6Setting->setIgnoreAutoDns(!mDnsList.isEmpty());
    }

    if (m_neverDefault->isVisible()) {
        ipv6Setting->setNeverDefault(m_neverDefault->checked());
    }

    return true;
}

void IpvxSection::setIpv4ConfigMethodEnable(NetworkManager::Ipv4Setting::ConfigMethod method, const bool enabled)
{
    if (!Ipv4ConfigMethodStrMap.values().contains(method)) {
        qDebug() << "Unsupport ipv4 config method" << method;
        return;
    }

    if (enabled) {
        m_methodChooser->appendOption(Ipv4ConfigMethodStrMap.key(method), method);
    } else {
        m_methodChooser->removeOption(method);
    }
}

void IpvxSection::setIpv6ConfigMethodEnable(NetworkManager::Ipv6Setting::ConfigMethod method, const bool enabled)
{
    if (!Ipv6ConfigMethodStrMap.values().contains(method)) {
        qDebug() << "Unsupport ipv6 config method" << method;
        return;
    }

    if (enabled) {
        m_methodChooser->appendOption(Ipv6ConfigMethodStrMap.key(method), method);
    } else {
        m_methodChooser->removeOption(method);
    }
}

void IpvxSection::setNeverDefaultEnable(const bool neverDefault)
{
    m_neverDefault->setVisible(neverDefault);
}

void IpvxSection::initStrMaps()
{
    Ipv4ConfigMethodStrMap = {
        {tr("Auto"), NetworkManager::Ipv4Setting::ConfigMethod::Automatic},
        {tr("Manual"), NetworkManager::Ipv4Setting::ConfigMethod::Manual}
    };

    Ipv6ConfigMethodStrMap = {
        {tr("Auto"), NetworkManager::Ipv6Setting::ConfigMethod::Automatic},
        {tr("Manual"), NetworkManager::Ipv6Setting::ConfigMethod::Manual},
        {tr("Ignore"), NetworkManager::Ipv6Setting::ConfigMethod::Ignored}
    };
}

void IpvxSection::initUI()
{
    m_methodChooser->setTitle(tr("Method"));
    m_ipAddress->setTitle(tr("IP Address"));
    m_ipAddress->textEdit()->setPlaceholderText(tr("Required"));
    m_gateway->setTitle(tr("Gateway"));
    m_dnsPrimary->setTitle(tr("Primary DNS"));
    m_dnsSecond->setTitle(tr("Secondary DNS"));
    m_neverDefault->setTitle(tr("Only applied in corresponding resources"));
    m_neverDefault->setVisible(false);

    appendItem(m_methodChooser);
    appendItem(m_ipAddress);

    switch (m_currentIpvx) {
        case Ipv4:
            initForIpv4();
            break;
        case Ipv6:
            initForIpv6();
            break;
        default:
            break;
    }

    appendItem(m_gateway);
    appendItem(m_dnsPrimary);
    appendItem(m_dnsSecond);
    appendItem(m_neverDefault);
}

void IpvxSection::initForIpv4()
{
    NetworkManager::Ipv4Setting::Ptr ipv4Setting = m_ipvxSetting.staticCast<NetworkManager::Ipv4Setting>();

    for (const QString &key : Ipv4ConfigMethodStrMap.keys()) {
        m_methodChooser->appendOption(key, Ipv4ConfigMethodStrMap.value(key));
    }

    if (Ipv4ConfigMethodStrMap.values().contains(ipv4Setting->method())) {
        m_methodChooser->setCurrent(ipv4Setting->method());
    } else {
        m_methodChooser->setCurrent(Ipv4ConfigMethodStrMap.first());
    }

    QList<NetworkManager::IpAddress> ipAddressList = ipv4Setting->addresses();
    if (!ipAddressList.isEmpty()) {
        // !! use the first ipaddress of list
        NetworkManager::IpAddress ipAddress = ipAddressList.first();
        m_ipAddress->setText(ipAddress.ip().toString());
        m_netmaskIpv4->setText(ipAddress.netmask().toString());
        const QString &gateStr = ipAddress.gateway().toString();
        m_gateway->setText(isIpv4Address(gateStr) ? gateStr : "");
    } else {
        m_ipAddress->setText("0.0.0.0");
        m_netmaskIpv4->setText("255.255.255.0");
    }

    const QList<QHostAddress> &dns = ipv4Setting->dns();
    for (int i = 0; i < dns.size(); ++i) {
        if (i == 0) {
            m_dnsPrimary->setText(dns.at(i).toString());
        }
        if (i == 1) {
            m_dnsSecond->setText(dns.at(i).toString());
        }
    }

    m_netmaskIpv4->setTitle(tr("Netmask"));
    m_netmaskIpv4->textEdit()->setPlaceholderText(tr("Required"));
    appendItem(m_netmaskIpv4);

    m_neverDefault->setChecked(ipv4Setting->neverDefault());

    onIpv4MethodChanged(Ipv4ConfigMethodStrMap.value(m_methodChooser->value()));
}

void IpvxSection::initForIpv6()
{
    m_prefixIpv6->setTitle(tr("Prefix"));
    m_prefixIpv6->spinBox()->setRange(0, 128);

    NetworkManager::Ipv6Setting::Ptr ipv6Setting = m_ipvxSetting.staticCast<NetworkManager::Ipv6Setting>();

    for (const QString &key : Ipv6ConfigMethodStrMap.keys()) {
        m_methodChooser->appendOption(key, Ipv6ConfigMethodStrMap.value(key));
    }

    if (Ipv6ConfigMethodStrMap.values().contains(ipv6Setting->method())) {
        m_methodChooser->setCurrent(ipv6Setting->method());
    } else {
        m_methodChooser->setCurrent(Ipv6ConfigMethodStrMap.first());
    }

    QList<NetworkManager::IpAddress> ipAddressList = ipv6Setting->addresses();
    if (!ipAddressList.isEmpty()) {
        // !! use the first ipaddress of list
        NetworkManager::IpAddress ipAddress = ipAddressList.first();
        m_ipAddress->setText(ipAddress.ip().toString());
        m_prefixIpv6->spinBox()->setValue(ipAddress.prefixLength());
        const QString &gateStr = ipAddress.gateway().toString();
        m_gateway->setText(isIpv6Address(gateStr) ? gateStr : "");
    }

    const QList<QHostAddress> &dns = ipv6Setting->dns();
    for (int i = 0; i < dns.size(); ++i) {
        if (i == 0) {
            m_dnsPrimary->setText(dns.at(i).toString());
        }
        if (i == 1) {
            m_dnsSecond->setText(dns.at(i).toString());
        }
    }

    appendItem(m_prefixIpv6);

    m_neverDefault->setChecked(ipv6Setting->neverDefault());

    onIpv6MethodChanged(Ipv6ConfigMethodStrMap.value(m_methodChooser->value()));
}

void IpvxSection::initConnection()
{
    connect(m_methodChooser, &ComboBoxWidget::requestPage, this, &IpvxSection::requestNextPage);
    connect(m_ipAddress->textEdit(), &QLineEdit::editingFinished, this, &IpvxSection::allInputValid);
    connect(m_gateway->textEdit(), &QLineEdit::editingFinished, this, &IpvxSection::allInputValid);
    connect(m_dnsPrimary->textEdit(), &QLineEdit::editingFinished, this, &IpvxSection::allInputValid);
    connect(m_dnsSecond->textEdit(), &QLineEdit::editingFinished, this, &IpvxSection::allInputValid);

    switch (m_currentIpvx) {
        case Ipv4:
            connect(m_methodChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
                onIpv4MethodChanged(data.value<NetworkManager::Ipv4Setting::ConfigMethod>());
            });
            connect(m_netmaskIpv4->textEdit(), &QLineEdit::editingFinished, this, &IpvxSection::allInputValid);
            break;
        case Ipv6:
            connect(m_methodChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
                onIpv6MethodChanged(data.value<NetworkManager::Ipv6Setting::ConfigMethod>());
            });
            connect(m_prefixIpv6->spinBox(), static_cast<void (QSpinBox:: *) (int)>(&QSpinBox::valueChanged),
                    this, &IpvxSection::allInputValid);
            break;
        default:
            break;
    }
}

void IpvxSection::onIpv4MethodChanged(NetworkManager::Ipv4Setting::ConfigMethod method)
{
    switch (method) {
        case NetworkManager::Ipv4Setting::Automatic:
            m_ipAddress->setVisible(false);
            m_netmaskIpv4->setVisible(false);
            m_gateway->setVisible(false);
            break;
        case NetworkManager::Ipv4Setting::Manual:
            m_ipAddress->setVisible(true);
            m_netmaskIpv4->setVisible(true);
            m_gateway->setVisible(true);
            break;
        default:
            break;
    }
}

void IpvxSection::onIpv6MethodChanged(NetworkManager::Ipv6Setting::ConfigMethod method)
{
    switch (method) {
        case NetworkManager::Ipv6Setting::Automatic:
            m_ipAddress->setVisible(false);
            m_prefixIpv6->setVisible(false);
            m_gateway->setVisible(false);
            m_dnsPrimary->setVisible(true);
            m_dnsSecond->setVisible(true);
            break;
        case NetworkManager::Ipv6Setting::Manual:
            m_ipAddress->setVisible(true);
            m_prefixIpv6->setVisible(true);
            m_gateway->setVisible(true);
            m_dnsPrimary->setVisible(true);
            m_dnsSecond->setVisible(true);
            break;
        case NetworkManager::Ipv6Setting::Ignored:
            m_ipAddress->setVisible(false);
            m_prefixIpv6->setVisible(false);
            m_gateway->setVisible(false);
            m_dnsPrimary->setVisible(false);
            m_dnsSecond->setVisible(false);
            break;
        default:
            break;
    }
}

bool IpvxSection::ipv4InputIsValid()
{
    bool valid = true;

    if (Ipv4ConfigMethodStrMap.value(m_methodChooser->value()) == NetworkManager::Ipv4Setting::Manual) {
        const QString &ip = m_ipAddress->text();
        if (!isIpv4Address(ip)) {
            valid = false;
            m_ipAddress->setIsErr(true);
        } else {
            m_ipAddress->setIsErr(false);
        }

        const QString &netmask = m_netmaskIpv4->text();
        if (!isIpv4SubnetMask(netmask)) {
            valid = false;
            m_netmaskIpv4->setIsErr(true);
        } else {
            m_netmaskIpv4->setIsErr(false);
        }

        const QString &gateway = m_gateway->text();
        if (!gateway.isEmpty() && !isIpv4Address(gateway)) {
            valid = false;
            m_gateway->setIsErr(true);
        } else {
            m_gateway->setIsErr(false);
        }
    }

    const QString &dnsPri = m_dnsPrimary->text();
    if (!dnsPri.isEmpty() && !isIpv4Address(dnsPri)) {
        valid = false;
        m_dnsPrimary->setIsErr(true);
    } else {
        m_dnsPrimary->setIsErr(false);
    }

    const QString &dnsSec = m_dnsSecond->text();
    if (!dnsSec.isEmpty() && !isIpv4Address(dnsSec)) {
        valid = false;
        m_dnsSecond->setIsErr(true);
    } else {
        m_dnsSecond->setIsErr(false);
    }

    return valid;
}

bool IpvxSection::ipv6InputIsValid()
{
    bool valid = true;

    if (Ipv6ConfigMethodStrMap.value(m_methodChooser->value()) == NetworkManager::Ipv6Setting::Ignored) {
        return valid;
    }

    if (Ipv6ConfigMethodStrMap.value(m_methodChooser->value()) == NetworkManager::Ipv6Setting::Manual) {
        const QString &ip = m_ipAddress->text();
        if (!isIpv6Address(ip)) {
            valid = false;
            m_ipAddress->setIsErr(true);
        } else {
            m_ipAddress->setIsErr(false);
        }

        if (m_prefixIpv6->spinBox()->value() == 0) {
            valid = false;
            m_prefixIpv6->setIsErr(true);
        } else {
            m_prefixIpv6->setIsErr(false);
        }

        const QString &gateway = m_gateway->text();
        if (!gateway.isEmpty() && !isIpv6Address(gateway)) {
            valid = false;
            m_gateway->setIsErr(true);
        } else {
            m_gateway->setIsErr(false);
        }
    }

    const QString &dnsPri = m_dnsPrimary->text();
    if (!dnsPri.isEmpty() && !isIpv6Address(dnsPri)) {
        valid = false;
        m_dnsPrimary->setIsErr(true);
    } else {
        m_dnsPrimary->setIsErr(false);
    }

    const QString &dnsSec = m_dnsSecond->text();
    if (!dnsSec.isEmpty() && !isIpv6Address(dnsSec)) {
        valid = false;
        m_dnsSecond->setIsErr(true);
    } else {
        m_dnsSecond->setIsErr(false);
    }

    return valid;
}

bool IpvxSection::isIpv4Address(const QString &ip)
{
    QHostAddress ipAddr(ip);
    if (ipAddr == QHostAddress(QHostAddress::Null) || ipAddr == QHostAddress(QHostAddress::AnyIPv4)
            || ipAddr.protocol() != QAbstractSocket::NetworkLayerProtocol::IPv4Protocol) {
        return false;
    }
    return true;
}

bool IpvxSection::isIpv6Address(const QString &ip)
{
    QHostAddress ipAddr(ip);
    if (ipAddr == QHostAddress(QHostAddress::Null) || ipAddr == QHostAddress(QHostAddress::AnyIPv6)
            || ipAddr.protocol() != QAbstractSocket::NetworkLayerProtocol::IPv6Protocol) {
        return false;
    }
    return true;
}

bool IpvxSection::isIpv4SubnetMask(const QString &ip)
{
    bool done;
    quint32 mask = QHostAddress(ip).toIPv4Address(&done);

    if (!done) {
        return false;
    }

    for (;mask != 0; mask <<= 1) {
        if ((mask & (1<<31)) == 0)
            return false; // Highest bit is now zero, but mask is non-zero.
    }
    return true; // Mask was, or became 0.
}

QList<QHostAddress> IpvxSection::dnsList()
{
    QList<QHostAddress> dnsList;
    const QString &dnsP = m_dnsPrimary->text();
    const QString &dnsS = m_dnsSecond->text();
    if (!dnsP.isEmpty()) {
        dnsList.append(QHostAddress(dnsP));
    }
    if (!dnsS.isEmpty()) {
        dnsList.append(QHostAddress(dnsS));
    }

    return dnsList;
}
