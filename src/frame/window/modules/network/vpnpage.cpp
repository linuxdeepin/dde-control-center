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
#include "connectionvpneditpage.h"
#include "widgets/contentwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/translucentframe.h"
#include "widgets/optionitem.h"
#include "widgets/nextpagewidget.h"
#include "widgets/loadingnextpagewidget.h"
#include "window/utils.h"

#include <networkmodel.h>
#include <DFloatingButton>
#include <DHiDPIHelper>
#include <ddialog.h>

#include <QDebug>
#include <QList>
#include <QMap>
#include <QVBoxLayout>
#include <QPushButton>
#include <QJsonObject>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardItemModel>

DWIDGET_USE_NAMESPACE

using namespace dcc;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::network;
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
    : QWidget(parent)
    , m_vpnSwitch(new SwitchWidget)
    , m_lvprofiles(new DListView)
    , m_modelprofiles(new QStandardItemModel)
{
    m_lvprofiles->setModel(m_modelprofiles);
    m_lvprofiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_vpnSwitch->setTitle(tr("VPN Status"));

    SettingsGroup *switchGrp = new SettingsGroup;
    switchGrp->appendItem(m_vpnSwitch);

    QVBoxLayout *scrollLayout = new QVBoxLayout;
    scrollLayout->addWidget(switchGrp);
    scrollLayout->addWidget(m_lvprofiles);
    scrollLayout->setSpacing(10);
    scrollLayout->setContentsMargins(ThirdPageContentsMargins);

    QWidget *widget = new QWidget(this);
    widget->setLayout(scrollLayout);

    ContentWidget *contentWidget = new ContentWidget(this);
    contentWidget->layout()->setMargin(0);
    contentWidget->setContent(widget);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(contentWidget);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->setSpacing(30);
    buttonsLayout->addStretch();

    DFloatingButton *createVpnBtn = new DFloatingButton(DStyle::StandardPixmap::SP_IncreaseElement);
    createVpnBtn->setMinimumSize(QSize(47, 47));
    buttonsLayout->addWidget(createVpnBtn);

    DFloatingButton *importVpnBtn = new DFloatingButton("\342\206\223");
    importVpnBtn->setMinimumSize(QSize(47, 47));
    buttonsLayout->addWidget(importVpnBtn);
    buttonsLayout->addStretch();
    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);

    connect(m_vpnSwitch, &SwitchWidget::checkedChanged, this, &VpnPage::requestVpnEnabled);
    connect(createVpnBtn, &QPushButton::clicked, this, &VpnPage::createVPN);
    connect(importVpnBtn, &QPushButton::clicked, this, &VpnPage::importVPN);
    connect(m_lvprofiles, &DListView::clicked, this, &VpnPage::onVpnSelected);
}

VpnPage::~VpnPage()
{
    if (!m_editPage.isNull())
        m_editPage->deleteLater();
}

void VpnPage::setModel(NetworkModel *model)
{
    m_model = model;

    connect(m_model, &NetworkModel::vpnEnabledChanged, m_vpnSwitch, &SwitchWidget::setChecked);
    connect(m_model, &NetworkModel::activeConnInfoChanged, this, &VpnPage::onActiveConnsInfoChanged);
    connect(m_model, &NetworkModel::connectionListChanged, this, [ = ] {
        refreshVpnList(m_model->vpns());
    });

    m_vpnSwitch->setChecked(m_model->vpnEnabled());

    refreshVpnList(m_model->vpns());
}

void VpnPage::refreshVpnList(const QList<QJsonObject> &vpnList)
{
    // NOTE: vpn name may changed
//    if (vpnList.size() == m_vpnGroup->itemCount())
//        return;

    m_modelprofiles->clear();

    for (const auto &vpn : vpnList) {
        const QString uuid = vpn.value("Uuid").toString();

        DStandardItem *it = new DStandardItem();
        it->setText(vpn.value("Id").toString());
        it->setData(QVariant::fromValue(vpn), VpnInfoRole);

        DViewItemAction *editaction = new DViewItemAction(Qt::AlignmentFlag::AlignCenter, QSize(11, 11), QSize(), true);
        QStyleOption opt;
        editaction->setIcon(DStyleHelper(style()).standardIcon(DStyle::SP_ArrowEnter, &opt, nullptr));

        connect(editaction, &QAction::triggered, [this, uuid] {
            this->onVpnDetailClicked(uuid);
        });
        it->setActionList(Qt::Edge::RightEdge, {editaction});

        m_modelprofiles->appendRow(it);
    }

    onActiveConnsInfoChanged(m_model->activeConnInfos());
    m_vpnSwitch->setVisible(m_modelprofiles->rowCount() > 0);
}

void VpnPage::onVpnDetailClicked(const QString &connectionUuid)
{
    m_editPage = new ConnectionVpnEditPage(connectionUuid);
    m_editPage->initSettingsWidget();
    connect(m_editPage, &ConnectionVpnEditPage::requestNextPage, this, &VpnPage::requestNextPage);
    connect(m_editPage, &ConnectionVpnEditPage::requestFrameAutoHide, this, &VpnPage::requestFrameKeepAutoHide);
    Q_EMIT requestNextPage(m_editPage);
}

void VpnPage::onVpnSelected(const QModelIndex &idx)
{
    const auto info = idx.data(VpnInfoRole).value<QJsonObject>();
    const QString uuid = info.value("Uuid").toString();

    Q_EMIT requestActivateConnection("/", uuid);
}

void VpnPage::onActiveConnsInfoChanged(const QList<QJsonObject> &infos)
{
    QMap<QString, int> activeVpnStates;

    for (const auto &info : infos) {
        const QString &type = info.value("ConnectionType").toString();
        if (!type.startsWith("vpn"))
            continue;

        const QString &uuid = info.value("ConnectionUuid").toString();
        const int state = m_model->activeConnObjectByUuid(uuid).value("State").toInt();

        activeVpnStates.insert(uuid, state);
    }

    for (int i = 0; i < m_modelprofiles->rowCount(); ++i) {
        QStandardItem *it = m_modelprofiles->item(i);
        const QString &uuid = it->data(VpnInfoRole).value<QJsonObject>().value("Uuid").toString();
        const bool exist = activeVpnStates.contains(uuid);
        const bool loading = exist && activeVpnStates[uuid] != 2;

        it->setCheckState((exist && !loading) ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

        //TODO: loading state?
        //it.key()->setLoading(loading);
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

    if (stat) {
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

    if (match.hasMatch()) {
        m_editingConnUuid = match.captured(1);
    }
}

void VpnPage::createVPN()
{
    m_editPage = new ConnectionVpnEditPage();
    m_editPage->initSettingsWidgetByType(ConnectionVpnEditPage::VpnType::UNSET);
    connect(m_editPage, &ConnectionVpnEditPage::requestNextPage, this, &VpnPage::requestNextPage);
    connect(m_editPage, &ConnectionVpnEditPage::requestFrameAutoHide, this, &VpnPage::requestFrameKeepAutoHide);
    Q_EMIT requestNextPage(m_editPage);
}
