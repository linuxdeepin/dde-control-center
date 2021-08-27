/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
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

#include "connectionvpneditpage.h"
#include "settings/vpn/vpnl2tpsettings.h"
#include "settings/vpn/vpnpptpsettings.h"
#include "settings/vpn/vpnvpncsettings.h"
#include "settings/vpn/vpnopenvpnsettings.h"
#include "settings/vpn/vpnstrongswansettings.h"
#include "settings/vpn/vpnopenconnectsettings.h"
#include "settings/vpn/vpnsstpsettings.h"

#include "widgets/comboxwidget.h"
#include "widgets/titlelabel.h"

#include <networkmanagerqt/vpnsetting.h>

#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QProcess>
#include <QRegularExpression>
#include <QHBoxLayout>

#include <widgets/comboxwidget.h>
#include <widgets/titlelabel.h>
#include <widgets/settingsgroup.h>

using namespace dcc;
using namespace dcc::widgets;
using namespace NetworkManager;

const QList<ConnectionVpnEditPage::VpnType> SupportedExportVpnList {
    ConnectionVpnEditPage::VpnType::L2TP,
    ConnectionVpnEditPage::VpnType::OPENVPN,
};

ConnectionVpnEditPage::ConnectionVpnEditPage(const QString &connUuid, QWidget *parent)
    : ConnectionEditPage(ConnectionEditPage::ConnectionType::VpnConnection, QString(), connUuid, parent)
    , m_exportButton(nullptr)
    , m_saveConfig(new QFileDialog(this))
{
    m_saveConfig->setModal(true);
    m_saveConfig->setNameFilter("Config File (*.conf)");
    m_saveConfig->setAcceptMode(QFileDialog::AcceptSave);
    QStringList directory = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    if (!directory.isEmpty())
        m_saveConfig->setDirectory(directory.first());

    connect(m_saveConfig, &QFileDialog::finished, this, [ = ] (int result) {
        Q_EMIT requestFrameAutoHide(true);
        if (result == QFileDialog::Accepted) {
            QString file = m_saveConfig->selectedFiles().first();
            if (!file.endsWith(".conf"))
               file.append(".conf");

            const QString uuid = connectionUuid();
            const auto args = QStringList() << "connection" << "export" << uuid << file;
            qDebug() << Q_FUNC_INFO;

            QProcess p;
            p.start("nmcli", args);
            p.waitForFinished();
            qDebug() << p.readAllStandardOutput();
            qDebug() << p.readAllStandardError();

            // process ca
            processConfigCA(file);
        }
    });
}

ConnectionVpnEditPage::~ConnectionVpnEditPage()
{
    if (m_exportButton)
        m_exportButton->deleteLater();

    if (m_saveConfig)
        m_saveConfig->deleteLater();
}

void ConnectionVpnEditPage::initSettingsWidget()
{
    if (!m_connection || !m_connectionSettings)
        return;

    const QString &serviceType = m_connectionSettings->setting(Setting::SettingType::Vpn).staticCast<VpnSetting>()->serviceType();

    qDebug() << "using existing vpn connection, connection type:" << serviceType;

    if (serviceType == ServiceTypeL2TP)
        initSettingsWidgetByType(ConnectionVpnEditPage::VpnType::L2TP);
    else if (serviceType == ServiceTypePPTP)
        initSettingsWidgetByType(ConnectionVpnEditPage::VpnType::PPTP);
    else if (serviceType == ServiceTypeVPNC)
        initSettingsWidgetByType(ConnectionVpnEditPage::VpnType::VPNC);
    else if (serviceType == ServiceTypeOpenVPN)
        initSettingsWidgetByType(ConnectionVpnEditPage::VpnType::OPENVPN);
    else if (serviceType == ServiceTypeStrongSwan)
        initSettingsWidgetByType(ConnectionVpnEditPage::VpnType::STRONGSWAN);
    else if (serviceType == ServiceTypeOpenConnect)
        initSettingsWidgetByType(ConnectionVpnEditPage::VpnType::OPENCONNECT);
}

void ConnectionVpnEditPage::initSettingsWidgetByType(ConnectionVpnEditPage::VpnType vpnType)
{
    if (!m_connectionSettings)
        return;

    if (vpnType == VpnType::UNSET) {
        QHBoxLayout *titleLayout = new QHBoxLayout(this);
        TitleLabel *lbcaption = new TitleLabel(tr("New VPN"), this);
        titleLayout->addStretch();
        titleLayout->addWidget(lbcaption, 0, Qt::AlignCenter);
        titleLayout->addStretch();

        SettingsGroup *typeGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);
        ComboxWidget *cbvpntype = new ComboxWidget(tr("VPN Type"), typeGrp);
        typeGrp->getLayout()->setContentsMargins(8, 0, 8, 0);
        typeGrp->appendItem(cbvpntype);
        QComboBox *comboBox = cbvpntype->comboBox();
        comboBox->addItem(tr("L2TP"), VpnType::L2TP);
        comboBox->addItem(tr("PPTP"), VpnType::PPTP);
        comboBox->addItem(tr("OpenVPN"), VpnType::OPENVPN);
        comboBox->addItem(tr("OpenConnect"), VpnType::OPENCONNECT);
        comboBox->addItem(tr("StrongSwan"), VpnType::STRONGSWAN);
        comboBox->addItem(tr("VPNC"), VpnType::VPNC);

        comboBox->setCurrentIndex(0);

        connect(comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [ this, comboBox ] () {
            m_settingsWidget->deleteLater();
            m_settingsWidget = nullptr;
            VpnType vpnType = VpnType(comboBox->currentData().toInt());
            resetConnectionIdByType(vpnType);
            initSettingsWidgetByType(vpnType);
            setButtonTupleEnable(true);
        });

        m_settingsLayout->addLayout(titleLayout);
        m_settingsLayout->addWidget(typeGrp);

        initSettingsWidgetByType(VpnType::L2TP);
        return;
    }

    if (m_connectionSettings->id().isEmpty())
        resetConnectionIdByType(vpnType);

    VpnSetting::Ptr vpnSetting = m_connectionSettings->setting(Setting::SettingType::Vpn).staticCast<VpnSetting>();

    switch (vpnType) {
        case VpnType::L2TP: {
            vpnSetting->setServiceType(ServiceTypeL2TP);
            m_settingsWidget = new VpnL2tpSettings(m_connectionSettings, this);
            break;
        }
        case VpnType::PPTP: {
            vpnSetting->setServiceType(ServiceTypePPTP);
            m_settingsWidget = new VpnPPTPSettings(m_connectionSettings, this);
            break;
        }
        case VpnType::VPNC: {
            vpnSetting->setServiceType(ServiceTypeVPNC);
            m_settingsWidget = new VpnVPNCSettings(m_connectionSettings, this);
            break;
        }
        case VpnType::OPENVPN: {
            vpnSetting->setServiceType(ServiceTypeOpenVPN);
            m_settingsWidget = new VpnOpenVPNSettings(m_connectionSettings, this);
            break;
        }
        case VpnType::STRONGSWAN: {
            vpnSetting->setServiceType(ServiceTypeStrongSwan);
            m_settingsWidget = new VpnStrongSwanSettings(m_connectionSettings, this);
            break;
        }
        case VpnType::OPENCONNECT: {
            vpnSetting->setServiceType(ServiceTypeOpenConnect);
            m_settingsWidget = new VpnOpenConnectSettings(m_connectionSettings, this);
            break;
        }
        default:
            break;
    }

    connect(m_settingsWidget, &AbstractSettings::anyEditClicked, this, [ this ] {
        setButtonTupleEnable(true);
    });
    connect(m_settingsWidget, &AbstractSettings::requestNextPage, this, &ConnectionVpnEditPage::onRequestNextPage);
    connect(m_settingsWidget, &AbstractSettings::requestFrameAutoHide, this, &ConnectionEditPage::requestFrameAutoHide);

    m_settingsLayout->addWidget(m_settingsWidget);

    // add export button
    if (SupportedExportVpnList.contains(vpnType) && !connectionUuid().isEmpty()) {
        if (!m_exportButton) {
            m_exportButton = new QPushButton(this);
            m_exportButton->setText(tr("Export"));
            connect(m_exportButton, &QPushButton::clicked, this, &ConnectionVpnEditPage::exportConnConfig);
            addHeaderButton(m_exportButton);
        }
        m_exportButton->setVisible(true);
    } else {
        if (m_exportButton)
            m_exportButton->setVisible(false);
    }
}

void ConnectionVpnEditPage::resetConnectionIdByType(ConnectionVpnEditPage::VpnType vpnType)
{
    QString connName;
    switch (vpnType) {
        case VpnType::L2TP: {
            connName = tr("VPN L2TP %1");
            break;
        }
        case VpnType::PPTP: {
            connName = tr("VPN PPTP %1");
            break;
        }
        case VpnType::VPNC: {
            connName = tr("VPN VPNC %1");
            break;
        }
        case VpnType::OPENVPN: {
            connName = tr("VPN OpenVPN %1");
            break;
        }
        case VpnType::STRONGSWAN: {
            connName = tr("VPN StrongSwan %1");
            break;
        }
        case VpnType::OPENCONNECT: {
            connName = tr("VPN OpenConnect %1");
            break;
        }
        default:
            break;
    }
    m_connectionSettings->setId(connName.arg(connectionSuffixNum(connName)));
}

void ConnectionVpnEditPage::exportConnConfig()
{
    Q_EMIT requestFrameAutoHide(false);
    m_saveConfig->show();
}

void ConnectionVpnEditPage::processConfigCA(const QString &file)
{
    QFile f(file);
    f.open(QIODevice::ReadWrite);
    const QString data = f.readAll();
    f.seek(0);

    const QRegularExpression regex("^(?:ca\\s'(.+)'\\s*)$");
//    const QRegularExpression regex("^(?:ca\\s'(.+)'\\s*|CACert=(.+)|UserCertificate=(.+))$");
    QStringList ca_list;
    for (const auto &line : data.split('\n')) {
        const auto match = regex.match(line);
        if (match.hasMatch()) {
            for (int i = 1; i != match.capturedLength(); ++i) {
                const auto cap = match.captured(i);
                if (cap.isNull() || cap.isEmpty())
                    continue;
                ca_list << cap;
            }
        } else {
            f.write(line.toStdString().c_str());
            f.write("\n");
        }
    }
    f.write("\n");

    if (!ca_list.isEmpty()) {
        // write ca
        f.write("<ca>\n");
        for (const auto &ca : ca_list) {
            QFile caf(ca);
            caf.open(QIODevice::ReadOnly);
            f.write(caf.readAll());
            f.write("\n");
        }
        f.write("</ca>\n");
    }

    f.flush();
    f.close();
}
