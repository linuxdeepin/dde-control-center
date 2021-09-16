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
#include "widgets/titlelabel.h"
#include "widgets/translucentframe.h"
#include "widgets/optionitem.h"
#include "widgets/nextpagewidget.h"
#include "widgets/loadingnextpagewidget.h"
#include "window/utils.h"

#include <networkmodel.h>
#include <DFloatingButton>
#include <DHiDPIHelper>
#include <ddialog.h>
#include <networkmanagerqt/settings.h>
#include <DSpinner>

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
    , m_lvprofiles(new DListView)
    , m_modelprofiles(new QStandardItemModel(this))
{
    m_lvprofiles->setAccessibleName("List_vpnList");
    m_lvprofiles->setModel(m_modelprofiles);
    m_lvprofiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_lvprofiles->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_lvprofiles->setSelectionMode(QAbstractItemView::NoSelection);

    TitleLabel *lblTitle = new TitleLabel(tr("VPN Status"));
    DFontSizeManager::instance()->bind(lblTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_vpnSwitch = new SwitchWidget(nullptr, lblTitle);
    m_vpnSwitch->switchButton()->setAccessibleName(lblTitle->text());

    QVBoxLayout *scrollLayout = new QVBoxLayout;
    scrollLayout->addWidget(m_vpnSwitch);
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

    //~ contents_path /network/VPN/Create VPN
    tr("Create VPN");
    DFloatingButton *createVpnBtn = new DFloatingButton(DStyle::StandardPixmap::SP_IncreaseElement);
    createVpnBtn->setMinimumSize(QSize(47, 47));
    createVpnBtn->setToolTip(tr("Create VPN"));
    createVpnBtn->setAccessibleName(tr("Create VPN"));
    buttonsLayout->addWidget(createVpnBtn);

    //~ contents_path /network/VPN/Import VPN
    tr("Import VPN");
    DFloatingButton *importVpnBtn = new DFloatingButton("\342\206\223");
    importVpnBtn->setMinimumSize(QSize(47, 47));
    importVpnBtn->setToolTip(tr("Import VPN"));
    importVpnBtn->setAccessibleName(tr("Import VPN"));
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
    m_modelprofiles->clear();

    for (const auto &vpn : vpnList) {
        const QString uuid = vpn.value("Uuid").toString();
        DStandardItem *it = new DStandardItem();
        it->setText(vpn.value("Id").toString());
        it->setData(QVariant::fromValue(vpn), VpnInfoRole);

        DViewItemAction *editaction = new DViewItemAction(Qt::AlignCenter, QSize(), QSize(), true);
        QStyleOption opt;
        editaction->setIcon(DStyleHelper(style()).standardIcon(DStyle::SP_ArrowEnter, &opt, nullptr));
        editaction->setClickAreaMargins(ArrowEnterClickMargin);

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
    connect(m_editPage, &ConnectionVpnEditPage::requestRefreshVPNStatusSignal, this, &VpnPage::requestRefreshVPNStatus);
    //删除VPN
    connect(m_editPage, &ConnectionVpnEditPage::deleteConnectAP, m_model, &NetworkModel::deleteConnection);
    //断开连接
    connect(m_editPage, &ConnectionEditPage::disconnectAP, m_model, &NetworkModel::requestDisconnctAP);
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
        DStandardItem *it = dynamic_cast<DStandardItem *>(m_modelprofiles->item(i));
        const QString &uuid = it->data(VpnInfoRole).value<QJsonObject>().value("Uuid").toString();
        const bool exist = activeVpnStates.contains(uuid);

        if (it->actionList(Qt::RightEdge).size() > 1) {
            it->actionList(Qt::RightEdge).first()->widget()->deleteLater();
            it->actionList(Qt::RightEdge).last()->deleteLater();
        }
        it->actionList(Qt::RightEdge).first()->deleteLater();

        DViewItemActionList dActionList;
        DViewItemAction *editaction = new DViewItemAction(Qt::AlignCenter, QSize(), QSize(), true);
        QStyleOption opt;
        editaction->setIcon(DStyleHelper(style()).standardIcon(DStyle::SP_ArrowEnter, &opt, nullptr));
        editaction->setClickAreaMargins(ArrowEnterClickMargin);

        connect(editaction, &QAction::triggered, [this, uuid] {
            this->onVpnDetailClicked(uuid);
        });

        if (exist && activeVpnStates[uuid] == 2) {
            it->setCheckState(Qt::Checked);
        } else if (exist && activeVpnStates[uuid] == 1) {
            it->setCheckState(Qt::Unchecked);

            DViewItemAction *loadingAction = new DViewItemAction(Qt::AlignCenter | Qt::AlignRight, QSize(), QSize(), false);
            DSpinner *loadingIndicator = new DSpinner(m_lvprofiles->viewport());
            loadingIndicator->setFixedSize(20, 20);
            loadingAction->setWidget(loadingIndicator);
            loadingIndicator->start();
            loadingIndicator->show();
            dActionList.append(loadingAction);
            editaction->setVisible(false);
        } else {
            it->setCheckState(Qt::Unchecked);
        }

        dActionList.append(editaction);
        it->setActionList(Qt::RightEdge, dActionList);
    }
}

void VpnPage::changeVpnId()
{
    NetworkManager::Connection::List connList = NetworkManager::listConnections();
    QString importName = "";
    for (const auto &conn : connList) {
        if (conn->settings()->connectionType() == NetworkManager::ConnectionSettings::Vpn) {
            if (m_editingConnUuid == conn->uuid()) {
                importName = conn->name();
                break;
            }
        }
    }
    if (importName.isEmpty()) {
        QTimer::singleShot(10, this, &VpnPage::changeVpnId);
        return;
    }

    QString changeName = "";
    bool hasSameName = false;
    for (const auto &conn : connList) {
        const QString vpnName = conn->name();
        const QString vpnUuid = conn->uuid();
        if ((vpnName == importName) && (vpnUuid != m_editingConnUuid)) {
            changeName = importName + "(1)";
            hasSameName = true;
            break;
        }
    }
    if (!hasSameName) {
        return;
    }

    for (int index = 1; ; index++) {
        hasSameName = false;
        for (const auto &conn : connList) {
            QString vpnName = conn->name();
            if (vpnName == changeName) {
                changeName = importName + "(%1)";
                changeName = changeName.arg(index);
                hasSameName = true;
                break;
            }
        }
        if (!hasSameName) {
            break;
        }
    }
    NetworkManager::Connection::Ptr uuidConn = NetworkManager::findConnectionByUuid(m_editingConnUuid);
    if (uuidConn) {
        NetworkManager::ConnectionSettings::Ptr connSettings = uuidConn->settings();
        connSettings->setId(changeName);
        // update function saves the settings on the hard disk
        QDBusPendingReply<> reply = uuidConn->update(connSettings->toMap());
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "error occurred while updating the connection" << reply.error();
            return;
        }
        qDebug() << "find Connection By Uuid is success";
        return;
    }
}

void VpnPage::requestRefreshVPNStatus()
{
    //+ 当m_vpnSwitch为勾选状态时手动刷新连接，使本次修改生效！
    if (m_vpnSwitch->checked()) {
        QTimer::singleShot(100,this,[this](){
            m_vpnSwitch->setChecked(false);
            Q_EMIT requestVpnEnabled(false);
        });

        QTimer::singleShot(500,this,[this](){
            m_vpnSwitch->setChecked(true);
            Q_EMIT requestVpnEnabled(true);
        });
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

    QProcess p;
    p.start("nmcli", args);
    p.waitForFinished();
    const auto stat = p.exitCode();
    const QString output = p.readAllStandardOutput();
    QString error = p.readAllStandardError();

    qDebug() << stat << ",output:" << output << ",err:" << error;

    if (stat) {
        const auto ratio = devicePixelRatioF();
        QPixmap icon = QIcon::fromTheme("dialog-error").pixmap(QSize(48, 48) * ratio);
        icon.setDevicePixelRatio(ratio);

        DDialog dialog;
        dialog.setTitle(tr("Import Error"));
        dialog.setMessage(tr("File error"));
        dialog.addButton(tr("OK"));
        dialog.setIcon(icon);
        dialog.exec();

        return;
    }

    const QRegularExpression regexp("\\(\\w{8}(-\\w{4}){3}-\\w{12}\\)");
    const auto match = regexp.match(output);

    if (match.hasMatch()) {
        m_editingConnUuid = match.captured();
        m_editingConnUuid.replace("(", "");
        m_editingConnUuid.replace(")", "");
        qDebug() << "editing connection Uuid";
        QTimer::singleShot(10, this, &VpnPage::changeVpnId);
    }
}

void VpnPage::createVPN()
{
    if (m_editPage)  return;

    m_editPage = new ConnectionVpnEditPage();
    m_editPage->initSettingsWidgetByType(ConnectionVpnEditPage::VpnType::UNSET);
    connect(m_editPage, &ConnectionVpnEditPage::requestNextPage, this, &VpnPage::requestNextPage);
    connect(m_editPage, &ConnectionVpnEditPage::requestFrameAutoHide, this, &VpnPage::requestFrameKeepAutoHide);
    connect(m_editPage, &ConnectionVpnEditPage::requestRefreshVPNStatusSignal, this, &VpnPage::requestRefreshVPNStatus);
    //删除VPN
    connect(m_editPage, &ConnectionVpnEditPage::deleteConnectAP, m_model, &NetworkModel::deleteConnection);
    Q_EMIT requestNextPage(m_editPage);
}

void VpnPage::jumpPath(const QString &searchPath)
{
    if (searchPath == "Create VPN") {
        createVPN();
    }
    if (searchPath == "Import VPN") {
        importVPN();
    }
}
