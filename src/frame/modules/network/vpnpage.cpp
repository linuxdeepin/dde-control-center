/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             listenerri <listenerri@gmail.com>
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

#include "vpnpage.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/translucentframe.h"
#include "widgets/optionitem.h"
#include "widgets/nextpagewidget.h"
#include "widgets/loadingnextpagewidget.h"

#include <networkmodel.h>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QJsonObject>
#include <QFileDialog>
#include <QMessageBox>
#include <DHiDPIHelper>
#include <ddialog.h>
#include <QProcess>
#include <QRegularExpression>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;
using namespace dcc::network;
using namespace dde::network;

QString vpnConfigType(const QString &path)
{
    QFile f(path);
    f.open(QIODevice::ReadOnly);
    const QString content = f.readAll();

    if (content.contains("openconnect"))
        return "openconnect";
    if (content.contains("l2tp"))
        return "l2tp";
    if (content.startsWith("[main]"))
        return "vpnc";

    return "openvpn";
}

VpnPage::VpnPage(QWidget *parent)
    : ContentWidget(parent),

      m_vpnSwitch(new SwitchWidget),
      m_vpnGroup(new SettingsGroup),
      m_vpnTypePage(nullptr)
{
    m_vpnSwitch->setTitle(tr("VPN Status"));

    QPushButton *createVpnBtn = new QPushButton;
    createVpnBtn->setText(tr("Create VPN"));

    QPushButton *importVpnBtn = new QPushButton;
    importVpnBtn->setText(tr("Import VPN"));

    SettingsGroup *switchGrp = new SettingsGroup;
    switchGrp->appendItem(m_vpnSwitch);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(10);
    mainLayout->addWidget(switchGrp);
    mainLayout->addWidget(m_vpnGroup);
    mainLayout->addWidget(createVpnBtn);
    mainLayout->addWidget(importVpnBtn);
    mainLayout->addStretch();
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);
    setTitle(tr("VPN"));

    connect(m_vpnSwitch, &SwitchWidget::checkedChanged, this, &VpnPage::requestVpnEnabled);
    connect(createVpnBtn, &QPushButton::clicked, this, &VpnPage::createVPNSession);
    connect(importVpnBtn, &QPushButton::clicked, this, &VpnPage::importVPN);
}

VpnPage::~VpnPage()
{
    Q_ASSERT(!m_vpnTypePage);

    if (!m_editPage.isNull())
        m_editPage->deleteLater();
}

void VpnPage::setModel(NetworkModel *model)
{
    m_model = model;

    connect(m_model, &NetworkModel::vpnEnabledChanged, m_vpnSwitch, &SwitchWidget::setChecked);
    connect(m_model, &NetworkModel::activeConnInfoChanged, this, &VpnPage::onActiveConnsInfoChanged);
    connect(m_model, &NetworkModel::connectionListChanged, this, [=] { refreshVpnList(m_model->vpns()); });

    m_vpnSwitch->setChecked(m_model->vpnEnabled());

    refreshVpnList(m_model->vpns());
}

void VpnPage::refreshVpnList(const QList<QJsonObject> &vpnList)
{
    // NOTE: vpn name may changed
//    if (vpnList.size() == m_vpnGroup->itemCount())
//        return;

    m_vpnGroup->clear();
    qDeleteAll(m_vpns.keys());
    m_vpns.clear();

    for (const auto &vpn : vpnList)
    {
        LoadingNextPageWidget *w = new LoadingNextPageWidget;
        w->setTitle(vpn.value("Id").toString());

        connect(w, &NextPageWidget::acceptNextPage, this, &VpnPage::onVpnDetailClicked);
        connect(w, &NextPageWidget::selected, this, &VpnPage::onVpnSelected);

        m_vpns[w] = vpn;
        m_vpnGroup->appendItem(w);
    }

    onActiveConnsInfoChanged(m_model->activeConnInfos());
}

void VpnPage::onVpnDetailClicked()
{
    LoadingNextPageWidget *w = static_cast<LoadingNextPageWidget *>(sender());
    Q_ASSERT(w && m_vpns.contains(w));

    m_editingConnUuid = m_vpns[w].value("Uuid").toString();

    m_editPage = new ConnectionVpnEditPage(m_editingConnUuid);
    m_editPage->initSettingsWidget();
    connect(m_editPage, &ConnectionVpnEditPage::requestNextPage, this, &VpnPage::requestNextPage);
    connect(m_editPage, &ConnectionVpnEditPage::requestFrameAutoHide, this, &VpnPage::requestFrameKeepAutoHide);
    Q_EMIT requestNextPage(m_editPage);
}

void VpnPage::onVpnSelected()
{
    LoadingNextPageWidget *w = static_cast<LoadingNextPageWidget *>(sender());
    Q_ASSERT(w && m_vpns.contains(w));

    const auto info = m_vpns[w];
    const QString uuid = info.value("Uuid").toString();

    Q_EMIT requestActivateConnection("/", uuid);
}

void VpnPage::onVpnEditFinished()
{
    if (m_vpnTypePage)
        Q_EMIT m_vpnTypePage->back();
}

void VpnPage::onActiveConnsInfoChanged(const QList<QJsonObject> &infos)
{
    QMap<QString, int> activeVpnStates;

    for (const auto &info : infos)
    {
        const QString &type = info.value("ConnectionType").toString();
        if (!type.startsWith("vpn"))
            continue;

        const QString &uuid = info.value("ConnectionUuid").toString();
        const int state = m_model->activeConnObjectByUuid(uuid).value("State").toInt();

        activeVpnStates.insert(uuid, state);
    }

    for (auto it(m_vpns.cbegin()); it != m_vpns.cend(); ++it)
    {
        const QString &uuid = it.value()["Uuid"].toString();
        const bool exist = activeVpnStates.contains(uuid);
        const bool loading = exist && activeVpnStates[uuid] != 2;

        if (exist && !loading)
            it.key()->setIcon(DHiDPIHelper::loadNxPixmap(":/network/themes/dark/icons/select.svg"));
        else
            it.key()->clearValue();
        it.key()->setLoading(loading);
    }
}

void VpnPage::importVPN()
{
    Q_EMIT requestFrameKeepAutoHide(false);
    const auto file = QFileDialog::getOpenFileUrl(nullptr, "", QUrl::fromLocalFile(QDir::homePath()), "*.conf");
    Q_EMIT requestFrameKeepAutoHide(true);
    if (file.isEmpty())
        return;

    const auto args = QStringList { "connection", "import", "type", vpnConfigType(file.path()), "file", file.path() };
    qDebug() << args;

    QProcess p;
    p.start("nmcli", args);
    p.waitForFinished();
    const auto stat = p.exitCode();
    const QString output = p.readAllStandardOutput();
    QString error = p.readAllStandardError();

    qDebug() << stat << output << error;

    if (stat)
    {
        const auto ratio = devicePixelRatioF();
        QPixmap icon = QIcon::fromTheme("dialog-error").pixmap(QSize(48, 48) * ratio);
        icon.setDevicePixelRatio(ratio);

        DDialog dialog;
        dialog.setTitle(tr("Import Error"));
        dialog.setMessage(error.replace(QRegularExpression("[:：]\\s+"), ":\n"));
        dialog.addButton(tr("OK"));
        dialog.setIcon(icon, QSize(64, 64));
        dialog.exec();

        return;
    }

    const QRegularExpression regexp("\\(([-\\w]+)\\)");
    const auto match = regexp.match(output);

    if (match.hasMatch())
    {
        m_editingConnUuid = match.captured(1);
    }
}

void VpnPage::createVPNSession()
{
    if (!m_vpnTypePage)
    {
        OptionItem *l2tp = new OptionItem;
        l2tp->setContentsMargins(20, 0, 10, 0);
        l2tp->setTitle(tr("L2TP"));
        OptionItem *pptp = new OptionItem;
        pptp->setContentsMargins(20, 0, 10, 0);
        pptp->setTitle(tr("PPTP"));
        OptionItem *vpnc = new OptionItem;
        vpnc->setContentsMargins(20, 0, 10, 0);
        vpnc->setTitle(tr("VPNC"));
        OptionItem *openvpn = new OptionItem;
        openvpn->setContentsMargins(20, 0, 10, 0);
        openvpn->setTitle(tr("OpenVPN"));
        OptionItem *strongswan = new OptionItem;
        strongswan->setContentsMargins(20, 0, 10, 0);
        strongswan->setTitle(tr("StrongSwan"));
        OptionItem *openconnect = new OptionItem;
        openconnect->setContentsMargins(20, 0, 10, 0);
        openconnect->setTitle(tr("OpenConnect"));
        OptionItem *sstp = new OptionItem;
        sstp->setContentsMargins(20, 0, 10, 0);
        sstp->setTitle(tr("SSTP"));

        connect(l2tp, &OptionItem::selectedChanged, [=] { createVPN(ConnectionVpnEditPage::VpnType::L2TP); });
        connect(pptp, &OptionItem::selectedChanged, [=] { createVPN(ConnectionVpnEditPage::VpnType::PPTP); });
        connect(vpnc, &OptionItem::selectedChanged, [=] { createVPN(ConnectionVpnEditPage::VpnType::VPNC); });
        connect(openvpn, &OptionItem::selectedChanged, [=] { createVPN(ConnectionVpnEditPage::VpnType::OPENVPN); });
        connect(strongswan, &OptionItem::selectedChanged, [=] { createVPN(ConnectionVpnEditPage::VpnType::STRONGSWAN); });
        connect(openconnect, &OptionItem::selectedChanged, [=] { createVPN(ConnectionVpnEditPage::VpnType::OPENCONNECT); });
        connect(sstp, &OptionItem::selectedChanged, [=] { createVPN(ConnectionVpnEditPage::VpnType::SSTP); });

        TranslucentFrame *widget = new TranslucentFrame;
        QVBoxLayout *mainLayout = new QVBoxLayout(widget);
        mainLayout->setMargin(0);

        SettingsGroup *grp = new SettingsGroup;
        grp->appendItem(l2tp);
        grp->appendItem(pptp);
        grp->appendItem(vpnc);
        grp->appendItem(openvpn);
        grp->appendItem(strongswan);
        grp->appendItem(openconnect);
        grp->appendItem(sstp);

        mainLayout->addSpacing(10);
        mainLayout->addWidget(grp);

        m_vpnTypePage = new ContentWidget;
        m_vpnTypePage->setTitle(tr("New VPN"));
        m_vpnTypePage->setContent(widget);

        connect(m_vpnTypePage, &ContentWidget::back, [=] {m_vpnTypePage = nullptr; });
    }

    Q_EMIT requestNextPage(m_vpnTypePage);
}

void VpnPage::createVPN(ConnectionVpnEditPage::VpnType vpnType)
{
    Q_ASSERT(m_vpnTypePage);

    m_editPage = new ConnectionVpnEditPage();
    m_editPage->initSettingsWidgetByType(vpnType);
    connect(m_editPage, &ConnectionVpnEditPage::requestNextPage, this, &VpnPage::requestNextPage);
    connect(m_editPage, &ConnectionVpnEditPage::requestFrameAutoHide, this, &VpnPage::requestFrameKeepAutoHide);
    connect(m_editPage, &ConnectionVpnEditPage::back, [=]{
        QTimer::singleShot(0, this, &VpnPage::onVpnEditFinished);
    });
    Q_EMIT requestNextPage(m_editPage);
}
