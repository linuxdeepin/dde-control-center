/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     donghualin <donghualin@gmail.com>
 *
 * Maintainer: donghualin <donghualin@gmail.com>
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

#include "multiipvxsection.h"
#include "widgets/contentwidget.h"

#include <DSpinBox>
#include <DPushButton>

#include <QDBusMetaType>
#include <QComboBox>

#include <widgets/comboxwidget.h>
#include <widgets/lineeditwidget.h>
#include <widgets/spinboxwidget.h>
#include <widgets/switchwidget.h>

#include <com_deepin_daemon_network.h>
#include <org_freedesktop_notifications.h>

Q_DECLARE_METATYPE(Ipv4Setting::ConfigMethod)
Q_DECLARE_METATYPE(Ipv6Setting::ConfigMethod)

using NetworkInter = com::deepin::daemon::Network;
using namespace dcc::widgets;
using namespace NetworkManager;

MultiIpvxSection::MultiIpvxSection(Setting::Ptr ipvSetting, QFrame *parent)
    : AbstractSection(parent)
    , m_ipvxSetting(ipvSetting)
    , m_mainFrame(new QFrame(this))
    , m_methodLine(new ComboxWidget(this))
    , m_headerEditWidget(new SettingsHead(this))
    , m_headerWidget(new SettingsHead(this))
    , m_isEditMode(false)
{
    setAccessibleName("MultiIpvxSection");
    setSettingsHead(m_headerEditWidget);
    setSettingsHead(m_headerWidget);
    m_headerWidget->setEditEnable(false);
    m_headerEditWidget->toCancel();
    m_headerWidget->setFixedHeight(30);
    m_headerEditWidget->setFixedHeight(30);

    m_methodChooser = m_methodLine->comboBox();
    m_methodLine->setTitle(tr("Method"));
    appendItem(m_methodLine);

    switch (ipvSetting->type()) {
    case Setting::SettingType::Ipv4:
        addIPV4Config();
        break;
    case Setting::SettingType::Ipv6:
        addIPV6Config();
        break;
    default: break;
    }

    QList<IPInputSection *> ipSections = createIpInputSections();
    for (IPInputSection *section : ipSections)
        setIpInputSection(section);

    setDefaultValue();

    onButtonShow(false);

    connect(m_headerEditWidget, &SettingsHead::editChanged, this, &MultiIpvxSection::onButtonShow);
}

MultiIpvxSection::~MultiIpvxSection()
{
}

bool MultiIpvxSection::allInputValid()
{
    if (m_ipvxSetting->type() == Setting::SettingType::Ipv4) {
        Ipv4Setting::ConfigMethod method = m_methodChooser->currentData().value<Ipv4Setting::ConfigMethod>();
        // 如果是自动模式，则直接返回true
        if (method != Ipv4Setting::ConfigMethod::Manual)
            return true;
    } else if (m_ipvxSetting->type() == Setting::SettingType::Ipv6) {
        Ipv6Setting::ConfigMethod method = m_methodChooser->currentData().value<Ipv6Setting::ConfigMethod>();
        // 如果是自动模式，则直接返回true
        if (method != Ipv6Setting::ConfigMethod::Manual)
            return true;
    }

    bool isValid = true;
    QList<IpAddress> allIpAddress;
    for (IPInputSection *section : m_ipSections) {
        if (section->allInputValid(allIpAddress))
            allIpAddress << section->ipAddress();
        else
            isValid = false;
    }

    return isValid;
}

void MultiIpvxSection::saveSettings()
{
    if (m_ipvxSetting->type() == Setting::SettingType::Ipv4) {
        Ipv4Setting::ConfigMethod method = m_methodChooser->currentData().value<Ipv4Setting::ConfigMethod>();
        // 如果是自动模式，则直接返回true
        Ipv4Setting::Ptr ipv4Setting = m_ipvxSetting.staticCast<Ipv4Setting>();
        ipv4Setting->setMethod(method);
        if (method == Ipv4Setting::ConfigMethod::Automatic) {
            QList<IpAddress>().clear();
            IpAddress ipAddressAuto;
            ipAddressAuto.setIp(QHostAddress(""));
            ipAddressAuto.setNetmask(QHostAddress(""));
            ipAddressAuto.setGateway(QHostAddress(""));
            ipv4Setting->setAddresses(QList<IpAddress>() << ipAddressAuto);
        } else if (method == Ipv4Setting::ConfigMethod::Manual) {
            QList<IpAddress> ipList;
            for (IPInputSection *ipSection : m_ipSections)
                ipList << ipSection->ipAddress();

            ipv4Setting->setAddresses(ipList);
        }
    } else if (m_ipvxSetting->type() == Setting::SettingType::Ipv6) {
        Ipv6Setting::ConfigMethod method = m_methodChooser->currentData().value<Ipv6Setting::ConfigMethod>();
        // 如果是自动模式，则直接返回true
        Ipv6Setting::Ptr ipv6Setting = m_ipvxSetting.staticCast<Ipv6Setting>();
        ipv6Setting->setMethod(method);
        if (method == Ipv6Setting::Ignored) {
            ipv6Setting->setAddresses(QList<IpAddress>());
            return;
        }
        if (method == Ipv6Setting::ConfigMethod::Manual) {
            QList<IpAddress> ipList;
            for (IPInputSection *ipSection : m_ipSections)
                ipList << ipSection->ipAddress();
            ipv6Setting->setAddresses(ipList);
        } else if (method == Ipv6Setting::ConfigMethod::Automatic) {
            QList<IpAddress> ipAddresses;
            ipAddresses.clear();
            IpAddress ipAddressAuto;
            ipAddressAuto.setIp(QHostAddress(""));
            ipAddressAuto.setPrefixLength(0);
            ipAddressAuto.setGateway(QHostAddress(""));
            ipAddresses.append(ipAddressAuto);
            ipv6Setting->setAddresses(ipAddresses);
        }
    }

    m_ipvxSetting->setInitialized(true);
}

void MultiIpvxSection::setDefaultValue()
{
    switch (m_ipvxSetting->type()) {
    case Setting::SettingType::Ipv4:
        onIPV4OptionChanged();
        break;
    case Setting::SettingType::Ipv6:
        onIPV6OptionChanged();
        break;
    default: break;
    }
}

void MultiIpvxSection::refreshItems()
{
    if (m_isEditMode) {
        bool isDeleteVisible = (m_ipSections.size() > 1);
        for(int i = 0; i < m_ipSections.size(); i++) {
            IPInputSection *section = m_ipSections[i];
            section->setTtile(QString("IP-%1").arg(i + 1));
            section->setDeleteItemVisible(isDeleteVisible);
            section->setAddItemVisible(true);
        }
    } else {
        for(int i = 0; i < m_ipSections.size(); i++) {
            IPInputSection *section = m_ipSections[i];
            section->setTtile(QString("IP-%1").arg(i + 1));
            section->setDeleteItemVisible(false);
            section->setAddItemVisible(false);
        }
    }
}

void MultiIpvxSection::addIPV4Config()
{
    m_headerEditWidget->setTitle(tr("IPv4"));
    m_headerWidget->setTitle(tr("IPv4"));
    Ipv4ConfigMethodStrMap = {
        { tr("Auto"), Ipv4Setting::ConfigMethod::Automatic },
        { tr("Manual"), Ipv4Setting::ConfigMethod::Manual }
    };

    for (const QString &key : Ipv4ConfigMethodStrMap.keys())
        m_methodChooser->addItem(key, Ipv4ConfigMethodStrMap.value(key));

    Ipv4Setting::Ptr ipv4Setting = m_ipvxSetting.staticCast<Ipv4Setting>();
    if (Ipv4ConfigMethodStrMap.values().contains(ipv4Setting->method()))
        m_methodChooser->setCurrentIndex(m_methodChooser->findData(ipv4Setting->method()));
    else
        m_methodChooser->setCurrentIndex(m_methodChooser->findData(Ipv4ConfigMethodStrMap.first()));

    connect(m_methodChooser, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MultiIpvxSection::onIPV4OptionChanged);
    // 改变选择的时候，需要让下面的确认按钮可用
    connect(m_methodChooser, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MultiIpvxSection::editClicked);
}

void MultiIpvxSection::addIPV6Config()
{
    m_headerEditWidget->setTitle(tr("IPv6"));
    m_headerWidget->setTitle(tr("IPv6"));
    Ipv6ConfigMethodStrMap = {
        { tr("Auto"), Ipv6Setting::ConfigMethod::Automatic },
        { tr("Manual"), Ipv6Setting::ConfigMethod::Manual },
        { tr("Ignore"), Ipv6Setting::ConfigMethod::Ignored }
    };

    for (const QString &key : Ipv6ConfigMethodStrMap.keys())
        m_methodChooser->addItem(key, Ipv6ConfigMethodStrMap.value(key));

    Ipv6Setting::Ptr ipv6Setting = m_ipvxSetting.staticCast<Ipv6Setting>();
    if (Ipv6ConfigMethodStrMap.values().contains(ipv6Setting->method()))
        m_methodChooser->setCurrentIndex(m_methodChooser->findData(ipv6Setting->method()));
    else
        m_methodChooser->setCurrentIndex(m_methodChooser->findData(Ipv6ConfigMethodStrMap.first()));

    connect(m_methodChooser, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MultiIpvxSection::onIPV6OptionChanged);
    // 改变选择的时候，需要让下面的确认按钮可用
    connect(m_methodChooser, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MultiIpvxSection::editClicked);
}

void MultiIpvxSection::onDeleteItem(IPInputSection *item)
{
    if (m_ipSections.contains(item)) {
        removeItem(item);
        m_ipSections.removeOne(item);
        delete item;
        refreshItems();
    }
}

QList<IPInputSection *> MultiIpvxSection::createIpInputSections()
{
    QList<IPInputSection *> ipInputSections;
    if (m_ipvxSetting->type() == Setting::SettingType::Ipv4) {
        Ipv4Setting::Ptr ipv4Setting = m_ipvxSetting.staticCast<Ipv4Setting>();
        QList<NetworkManager::IpAddress> addresses = ipv4Setting->addresses();
        if (addresses.isEmpty())
            addresses << NetworkManager::IpAddress();
        for (const NetworkManager::IpAddress &ipAddress : addresses)
            ipInputSections << new IPV4InputSection(ipAddress, m_mainFrame);
    } else if (m_ipvxSetting->type() == Setting::SettingType::Ipv6) {
        Ipv6Setting::Ptr ipv6Setting = m_ipvxSetting.staticCast<Ipv6Setting>();
        QList<NetworkManager::IpAddress> addresses = ipv6Setting->addresses();
        if (addresses.isEmpty())
            addresses << NetworkManager::IpAddress();

        for (const NetworkManager::IpAddress &ipAddress : addresses)
            ipInputSections << new IPV6InputSection(ipAddress, m_mainFrame);
    }

    return ipInputSections;
}

void MultiIpvxSection::setIpInputSection(IPInputSection *ipSection, IPInputSection *itemBefore)
{
    connect(ipSection, &IPV4InputSection::editClicked, this, &MultiIpvxSection::editClicked);
    connect(ipSection, &IPV4InputSection::requestDelete, this, &MultiIpvxSection::onDeleteItem);
    connect(ipSection, &IPV4InputSection::requestAdd, this, &MultiIpvxSection::onAddItem);
    int insertIndex = itemIndex(itemBefore);
    if (insertIndex < 0) {
        appendItem(ipSection);
        // 放入到列表的最后面
        m_ipSections << ipSection;
    } else {
        // 插入到当前IP的下方
        insertIndex++;
        insertItem(insertIndex, ipSection);
        // 插入到列表中的正确位置
        insertIndex = m_ipSections.indexOf(itemBefore) + 1;
        m_ipSections.insert(insertIndex, ipSection);
    }
}

void MultiIpvxSection::onAddItem(IPInputSection *item)
{
    if (m_ipvxSetting->type() == Setting::SettingType::Ipv4) {
        NetworkManager::IpAddress ipAddress;
        IPV4InputSection *ipSection = new IPV4InputSection(ipAddress, m_mainFrame);
        setIpInputSection(ipSection, item);
        refreshItems();
    } else if (m_ipvxSetting->type() == Setting::SettingType::Ipv6) {
        NetworkManager::IpAddress ipAddress;
        ipAddress.setPrefixLength(64);
        IPV6InputSection *ipSection = new IPV6InputSection(ipAddress, m_mainFrame);
        setIpInputSection(ipSection, item);
        refreshItems();
    }
}

void MultiIpvxSection::onIPV4OptionChanged()
{
    Ipv4Setting::ConfigMethod method = m_methodChooser->currentData().value<Ipv4Setting::ConfigMethod>();
    for (IPInputSection *section : m_ipSections) {
        IPV4InputSection *ipv4Section = qobject_cast<IPV4InputSection *>(section);
        if (ipv4Section)
            section->setVisible(method == Ipv4Setting::ConfigMethod::Manual);
    }
    if (method == Ipv4Setting::ConfigMethod::Manual) {
        m_headerEditWidget->setVisible(true);
        m_headerWidget->setVisible(false);
    } else {
        m_headerEditWidget->setVisible(false);
        m_headerWidget->setVisible(true);
    }
}

void MultiIpvxSection::onIPV6OptionChanged()
{
    Ipv6Setting::ConfigMethod method = m_methodChooser->currentData().value<Ipv6Setting::ConfigMethod>();
    for (IPInputSection *section : m_ipSections) {
        IPV6InputSection *ipv6Section = qobject_cast<IPV6InputSection *>(section);
        if (ipv6Section) {
            section->setVisible(method == Ipv6Setting::ConfigMethod::Manual);
        }
    }
    if (method == Ipv6Setting::ConfigMethod::Manual) {
        m_headerEditWidget->setVisible(true);
        m_headerWidget->setVisible(false);
    } else {
        m_headerEditWidget->setVisible(false);
        m_headerWidget->setVisible(true);
    }
}

void MultiIpvxSection::onButtonShow(bool edit)
{
    m_isEditMode = edit;
    refreshItems();
    Q_EMIT editClicked();
}

/**
 * @brief IP面板
 * @param ipv4Setting
 * @param parent
 */
IPInputSection::IPInputSection(IpAddress ipAddress, QFrame *parent)
    : SettingsItem(parent)
    , m_lineIpAddress(new LineEditWidget(this))
    , m_gateway(new LineEditWidget(this))
    , m_mainLayout(new QVBoxLayout(this))
    , m_ipAddress(ipAddress)
    , m_headerWidget(new QWidget(this))
    , m_titleLabel(new DLabel(this))
    , m_newIpButton(new ActionButton(ActionButton::Add, this))
    , m_pSpaceLabel(new QLabel(this))
    , m_deleteButton(new ActionButton(ActionButton::Delete, this))
{
    initUi();
    initConnection();
}

IPInputSection::~IPInputSection()
{
    m_lineIpAddress->textEdit()->disconnect();
}

void IPInputSection::setTtile(const QString &title)
{
    m_titleLabel->setText(title);
}

void IPInputSection::setAddItemVisible(bool visible)
{
    m_newIpButton->setVisible(visible);
}

void IPInputSection::setDeleteItemVisible(bool visible)
{
    m_deleteButton->setVisible(visible);
    m_pSpaceLabel->setVisible(visible);
}

void IPInputSection::initUi()
{
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    // 添加删除按钮
    m_deleteButton->setFixedSize(20, 20);

    m_pSpaceLabel->setFixedWidth(10);
    m_newIpButton->setFixedSize(20, 20);
    QHBoxLayout *headerLayout = new QHBoxLayout(m_headerWidget);
    headerLayout->setContentsMargins(10, 10, 10, 0);
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_newIpButton);
    headerLayout->addWidget(m_pSpaceLabel);
    headerLayout->addWidget(m_deleteButton);

    m_lineIpAddress->setTitle(tr("IP Address"));
    m_lineIpAddress->textEdit()->setPlaceholderText(tr("Required"));
    m_gateway->setTitle(tr("Gateway"));

    m_mainLayout->addWidget(m_headerWidget);
    m_mainLayout->addWidget(m_lineIpAddress);
    m_mainLayout->addWidget(m_gateway);

    setAddItemVisible(true);

    m_lineIpAddress->textEdit()->installEventFilter(this);
    m_gateway->textEdit()->installEventFilter(this);
}

void IPInputSection::initConnection()
{
    connect(m_lineIpAddress->textEdit(), &QLineEdit::textChanged, this, [ this ] {
        if (!m_lineIpAddress->text().isEmpty())
            m_lineIpAddress->dTextEdit()->setAlert(false);
    });

    connect(m_newIpButton, &ActionButton::clicked, this, [ this ] {
        Q_EMIT editClicked();
        Q_EMIT requestAdd(this);
    });
    connect(m_deleteButton, &ActionButton::clicked, this, [ this ] {
        Q_EMIT editClicked();
        Q_EMIT requestDelete(this);
    });
}

bool IPInputSection::eventFilter(QObject *watched, QEvent *event)
{
    // 实现鼠标点击编辑框，确定按钮激活，统一网络模块处理，捕捉FocusIn消息
    if (event->type() == QEvent::FocusIn) {
        if (dynamic_cast<QLineEdit *>(watched) || dynamic_cast<QSpinBox *>(watched))
            Q_EMIT editClicked();
    }

    return QWidget::eventFilter(watched, event);
}

IPV4InputSection::IPV4InputSection(IpAddress ipAddress, QFrame *parent)
    : IPInputSection(ipAddress, parent)
    , m_netmaskIpv4(new LineEditWidget(this))
{
    initUi();
    initConnection();
}

IPV4InputSection::~IPV4InputSection()
{
}

bool IPV4InputSection::allInputValid(const QList<IpAddress> &ipAddresses)
{
    bool valid = true;
    const QString &ip = m_lineIpAddress->text();
    if (m_lineIpAddress->text().isEmpty())
        m_lineIpAddress->dTextEdit()->setAlert(true);

    for (const IpAddress &address : ipAddresses) {
        if (address.ip().toString() == ip) {
            valid = false;
            m_lineIpAddress->textEdit()->setFocus();
            m_lineIpAddress->setIsErr(true);
            m_lineIpAddress->dTextEdit()->showAlertMessage(tr("Duplicate IP address"), m_lineIpAddress, 2000);
            break;
        }
    }

    if (!isIpv4Address(ip)) {
        valid = false;
        m_lineIpAddress->setIsErr(true);
        m_lineIpAddress->dTextEdit()->showAlertMessage(tr("Invalid IP address"), m_lineIpAddress, 2000);
    } else {
        m_lineIpAddress->setIsErr(false);
    }

    const QString &netmask = m_netmaskIpv4->text();
    if (m_netmaskIpv4->text().isEmpty())
        m_netmaskIpv4->dTextEdit()->setAlert(true);

    if (!isIpv4SubnetMask(netmask)) {
        valid = false;
        m_netmaskIpv4->setIsErr(true);
        m_netmaskIpv4->dTextEdit()->showAlertMessage(tr("Invalid netmask"), m_netmaskIpv4, 2000);
    } else {
        m_netmaskIpv4->setIsErr(false);
    }

    const QString &gateway = m_gateway->text();
    if (ipAddresses.size() > 0) {
        const IpAddress ipAddr = ipAddresses[0];
        if (gateway != "0.0.0.0" && !gateway.isEmpty() && ipAddr.gateway().toString() != gateway) {
            valid = false;
            m_gateway->setIsErr(true);
            m_gateway->dTextEdit()->showAlertMessage(tr("Only one gateway is allowed"), parentWidget(), 2000);
        } else {
            m_gateway->setIsErr(false);
        }
    }
    if (!gateway.isEmpty() && !isIpv4Address(gateway)) {
        valid = false;
        m_gateway->setIsErr(true);
        m_gateway->dTextEdit()->showAlertMessage(tr("Invalid gateway"), parentWidget(), 2000);
    } else {
        m_gateway->setIsErr(false);
    }

    return valid;
}

void IPV4InputSection::initUi()
{
    // !! use the first ipaddress of listIpv6Setting
    QString ip = m_ipAddress.ip().toString();
    if (ip.isEmpty())
        ip = QString("0.0.0.0");

    QString netmask = m_ipAddress.netmask().toString();
    if (netmask.isEmpty())
        netmask = QString("255.255.255.0");

    QString gateStr = m_ipAddress.gateway().toString();
    if (!isIpv4Address(gateStr))
        gateStr = "";

    m_lineIpAddress->setText(ip);
    m_netmaskIpv4->setText(netmask);
    m_gateway->setText(gateStr);

    m_netmaskIpv4->setTitle(tr("Netmask"));
    m_netmaskIpv4->textEdit()->setPlaceholderText(tr("Required"));

    m_mainLayout->insertWidget(2, m_netmaskIpv4);

    m_netmaskIpv4->textEdit()->installEventFilter(this);
}

void IPV4InputSection::initConnection()
{
    connect(m_netmaskIpv4->textEdit(), &QLineEdit::textChanged, this, [ this ] {
        if (!m_netmaskIpv4->text().isEmpty())
            m_netmaskIpv4->dTextEdit()->setAlert(false);
    });
}

bool IPV4InputSection::isIpv4Address(const QString &ip)
{
    QHostAddress ipAddr(ip);
    if (ipAddr == QHostAddress(QHostAddress::Null) || ipAddr == QHostAddress(QHostAddress::AnyIPv4)
            || ipAddr.protocol() != QAbstractSocket::NetworkLayerProtocol::IPv4Protocol) {
        return false;
    }

    QRegExp regExpIP("((25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])[\\.]){3}(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])");
    return regExpIP.exactMatch(ip);
}

bool IPV4InputSection::isIpv4SubnetMask(const QString &ip)
{
    bool done;
    quint32 mask = QHostAddress(ip).toIPv4Address(&done);

    if (!done)
        return false;

    for (; mask != 0; mask <<= 1) {
        if ((mask & (static_cast<uint>(1) << 31)) == 0)
            return false; // Highest bit is now zero, but mask is non-zero.
    }

    QRegExp regExpIP("^((128|192)|2(24|4[08]|5[245]))(\\.(0|(128|192)|2((24)|(4[08])|(5[245])))){3}$");
    return regExpIP.exactMatch(ip);
}

IpAddress IPV4InputSection::ipAddress()
{
    IpAddress ipAddr;
    ipAddr.setIp(QHostAddress(m_lineIpAddress->text()));
    ipAddr.setGateway(QHostAddress(m_gateway->text()));
    ipAddr.setNetmask(QHostAddress(m_netmaskIpv4->text()));
    return ipAddr;
}

/**
 * @brief IPV6输入
 * @param ipv4Setting
 * @param parent
 */
IPV6InputSection::IPV6InputSection(IpAddress ipAddress, QFrame *parent)
    : IPInputSection(ipAddress, parent)
    , m_prefixIpv6(new SpinBoxWidget(this))
{
    initUi();
}

IPV6InputSection::~IPV6InputSection()
{
}

bool IPV6InputSection::allInputValid(const QList<IpAddress> &ipAddresses)
{
    bool valid = true;

    const QString &ip = m_lineIpAddress->text();
    if (m_lineIpAddress->text().isEmpty())
        m_lineIpAddress->dTextEdit()->setAlert(true);

    // 此处需要先将IP地址转换成IpAddress再转回字符串，因为IPv6地址段中有某个段为0,则转回的字符串中就会把这个字符串删除导致两个地址不等
    IpAddress tmpIpAddr;
    tmpIpAddr.setIp(QHostAddress(ip));
    const QString sourceIp = tmpIpAddr.ip().toString();
    for (const IpAddress &address : ipAddresses) {
        if (address.ip().toString() == sourceIp) {
            valid = false;
            m_lineIpAddress->textEdit()->setFocus();
            m_lineIpAddress->setIsErr(true);
            m_lineIpAddress->dTextEdit()->showAlertMessage(tr("Duplicate IP address"), m_lineIpAddress, 2000);
            break;
        }
    }

    if (!isIpv6Address(ip)) {
        valid = false;
        m_lineIpAddress->setIsErr(true);
        m_lineIpAddress->dTextEdit()->showAlertMessage(tr("Invalid IP address"), m_lineIpAddress, 2000);
    } else {
        m_lineIpAddress->setIsErr(false);
    }

    if (m_prefixIpv6->spinBox()->value() == 0) {
        valid = false;
        m_prefixIpv6->setIsErr(true);
    } else {
        m_prefixIpv6->setIsErr(false);
    }

    const QString &gateway = m_gateway->text();
    IpAddress gateWayAddr;
    gateWayAddr.setGateway(QHostAddress(gateway));
    const QString formatGateway = gateWayAddr.gateway().toString();

    if (ipAddresses.size() > 0) {
        const IpAddress ipAddr = ipAddresses[0];
        if (!formatGateway.isEmpty() && ipAddr.gateway().toString() != formatGateway) {
            valid = false;
            m_gateway->setIsErr(true);
            m_gateway->dTextEdit()->showAlertMessage(tr("Only one gateway is allowed"), parentWidget(), 2000);
        } else {
            m_gateway->setIsErr(false);
        }
    }
    if (!gateway.isEmpty() && !isIpv6Address(gateway)) {
        valid = false;
        m_gateway->setIsErr(true);
        m_gateway->dTextEdit()->showAlertMessage(tr("Invalid gateway"), parentWidget(), 2000);
    } else {
        m_gateway->setIsErr(false);
    }

    return valid;
}

IpAddress IPV6InputSection::ipAddress()
{
    IpAddress ipAddr;
    ipAddr.setIp(QHostAddress(m_lineIpAddress->text()));
    ipAddr.setGateway(QHostAddress(m_gateway->text()));
    ipAddr.setPrefixLength(m_prefixIpv6->spinBox()->value());
    return ipAddr;
}

void IPV6InputSection::initUi()
{
    m_prefixIpv6->setTitle(tr("Prefix"));
    m_prefixIpv6->spinBox()->setRange(1, 128);
    m_prefixIpv6->setDefaultVal(64);

    // !! use the first ipaddress of list
    m_lineIpAddress->setText(m_ipAddress.ip().toString());
    if (m_ipAddress.prefixLength() > 0)
        m_prefixIpv6->spinBox()->setValue(m_ipAddress.prefixLength());
    const QString &gateStr = m_ipAddress.gateway().toString();
    m_gateway->setText(isIpv6Address(gateStr) ? gateStr : "");

    m_mainLayout->insertWidget(2, m_prefixIpv6);

    m_prefixIpv6->spinBox()->installEventFilter(this);
}

bool IPV6InputSection::isIpv6Address(const QString &ip)
{
    QHostAddress ipAddr(ip);
    if (ipAddr == QHostAddress(QHostAddress::Null) || ipAddr == QHostAddress(QHostAddress::AnyIPv6)
            || ipAddr.protocol() != QAbstractSocket::NetworkLayerProtocol::IPv6Protocol) {
        return false;
    }

    return (ipAddr != QHostAddress(QHostAddress::LocalHostIPv6));
}
